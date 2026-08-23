#include <renderer/rasterizer.hpp>
#include <renderer/math.hpp>
#include <renderer/typeDefs.hpp>
#include <iostream>
#include <limits>

namespace renderer {


    std::vector<uint32_t> rasterizer::rasterize(renderer::scene scene, int32_t height, int32_t width) {
        this->colorBuffer.resize(height*width);
        this->depthBuffer.resize(height*width);
        this->height = height;
        this->width = width;
        std::fill_n(this->colorBuffer.begin(), height*width, 0xFFFFFFFF);
        std::fill_n(this->depthBuffer.begin(), height*width, std::numeric_limits<float>::infinity());
        
        float aspectRatio = static_cast<float>(width)/height;

        for(renderer::model object : scene.models) {

            renderer::AlignedVec4 worldVerts = renderer::localToTransform(object.transform, object.vertexPositions);
            renderer::AlignedVec4 projectedVerts = renderer::getProjectedCoordinates(
                worldVerts, scene.cameraPos, scene.fov, scene.near, scene.far, aspectRatio);
            
            renderer::AlignedVec4 rasterVerts = renderer::getRasterCoords(projectedVerts, height, width);
            
            for(renderer::Face face : object.faceVerticies) {
                drawTri(rasterVerts[face.Verts[0]], rasterVerts[face.Verts[1]], rasterVerts[face.Verts[2]], face, object);
            }

            // int32_t i = 0;
            // for (renderer::vec3 point: rasterVerts) {
            //     int32_t x = static_cast<int32_t>(point.x);
            //     int32_t y = static_cast<int32_t>(point.y);
            //     //std::cout << i << "," << x << "," << y << "," << point.z << "\n";

            //     if (x >= width || x < 0 || y >= height || y < 0) {
            //         //std::cout << i << "," << x << "," << y << "\n";
            //         continue;
            //     }

            //     if (depthBuffer[(x) + (y*height)] > point.z) {
            //         //std::cout << i << "," << x << "," << y << "\n";
            //         depthBuffer[x + (y*width)] = point.z;
            //         float u = object.vertexUVs[i].x * 1024;
            //         float v = object.vertexUVs[i].y * 1024;
            //         uint32_t color = object.albedo[(static_cast<int>(u)) + (static_cast<int>(v) * 1024)];
            //         colorBuffer[x + (y*width)] = color;

            //         // colorBuffer[x+1 + (y*width)] = 0xFF000000;
            //         // colorBuffer[x-1 + (y*width)] = 0xFF000000;
            //         // colorBuffer[x+1 + ((y+1)*width)] = 0xFF000000;
            //         // colorBuffer[x+1 + ((y-1)*width)] = 0xFF000000;

            //     }
            //     ++i;
            // }
        }

        return this->colorBuffer;
    }
    
    

    //factoring out edges for readability is a nice touch. Thanks ryg.
    struct edge {
        static const int32_t stepXsize = 32;
        static const int32_t stepYsize = 32;

        int32x4_t oneStepX;
        int32x4_t oneStepY;

        int32x4_t init(std::array<int32_t,2> v0, std::array<int32_t,2> v1, std::array<int32_t,2> origin);
    };

    int32x4_t edge::init(std::array<int32_t,2> v0,std::array<int32_t,2> v1, std::array<int32_t,2> origin) {
        int32_t a = v0[1] - v1[1];
        int32_t b = v1[0] - v0[0];
        int32_t c = (((v0[0]*v1[1]) + 8) >> 4) - (((v0[1]*v1[0]) + 8) >> 4);

        // oneStepX = vmovq_n_s32((a * stepXsize) >> 4);
        oneStepX = vmovq_n_s32(a << 1);

        // oneStepY = vmovq_n_s32((b * stepYsize) >> 4);
        oneStepY = vmovq_n_s32(b << 1);

        int32x4_t offsetsX = {0,16,0,16};
        int32x4_t offsetsY = {0,0,16,16};

        int32x4_t A = vmovq_n_s32(a);
        int32x4_t x = vmovq_n_s32(origin[0]);
        x = vaddq_f32(x, offsetsX);
        x = vmulq_s32(x, A);
        x = vshrq_n_s32(x, 4);

        int32x4_t B = vmovq_n_s32(b);
        int32x4_t y = vmovq_n_s32(origin[1]);
        y = vaddq_s32(y, offsetsY);
        y = vmulq_s32(y, B);
        y = vshrq_n_s32(y, 4);


        int32x4_t C = vmovq_n_s32(c);
        C = vaddq_s32(C, y);
        C = vaddq_s32(C, x);

        return C;
    }
    
    int computeArea(std::array<int,2> const &v0, std::array<int,2> const &v1, std::array<int,2> const &v2) {
        return (((((v1[0] - v0[0]) * (v2[1] - v0[1])) + 8) >> 4) - ((((v1[1] - v0[1]) * (v2[0] - v0[0])) + 8 )>> 4));
    }

    //is there a way to do a simd bounding box with only 3 verticies?
    //if we just load our verts in AoS then doing two max and min calls could work?
    //but we're only really doing 2 sets at once
    //okay lets try that
    void rasterizer::drawTri(vec4 const &v0, vec4 const &v1, vec4 const &v2, Face const &face, model const &m) {
        //std::cout << "drawing face " << face.Verts[0] << " " << face.Verts[1] << " " << face.Verts[2] << "n";
        //need to quickly convert to fixed int32_t points
        //Process looks like it should be multiply by 16
        //Then convert, IE << 8 the whole number part so the fractional component sits in the lowest 4 bits
        //There is a neon intrinsic for fixed point conversion however it seems to use 16 bit fractional precision which is bad in this context

        
        float32x2_t scaling = vmov_n_f32(16);
        float32x2_t V0 = vld1_f32(reinterpret_cast<float const *>(&v0));
        float32x2_t V1 = vld1_f32(reinterpret_cast<float const *>(&v1));
        float32x2_t V2 = vld1_f32(reinterpret_cast<float const *>(&v2));
        V0 = vmul_f32(V0, scaling);
        V1 = vmul_f32(V1, scaling);
        V2 = vmul_f32(V2, scaling);
        
        int32x2_t P0 = vcvtm_s32_f32(V0);
        int32x2_t P1 = vcvtm_s32_f32(V1);
        int32x2_t P2 = vcvtm_s32_f32(V2);
        
        std::array<int32_t,2> p0;
        vst1_s32(p0.data(), P0);
        std::array<int32_t,2> p1;
        vst1_s32(p1.data(), P1);
        std::array<int32_t,2> p2;
        vst1_s32(p2.data(), P2);
        
        std::array<int32_t,2> minV;
        int32x2_t zero = vmov_n_s32(0);
        int32x2_t min = vmin_s32(P0, P1);
        min = vmin_s32(min, P2);
        min = vmax_s32(min, zero);
        vst1_s32(minV.data(), min);
        
        std::array<int32_t,2> maxV;
        int32x2_t max = vmax_s32(P0, P1);
        int32x2_t screen = {((this->width - 1) << 4), ((this->height - 1) << 4)};
        max = vmax_s32(max, P2);
        max = vmin_s32(max, screen);
        vst1_s32(maxV.data(), max);

        minV[0] = ((minV[0] + 15)  & (~15));
        minV[1] = ((minV[1] + 15)  & (~15));
        
        int area = computeArea(p2, p1, p0);
        if (area < 0) {
            return;
        }
        float oneOnArea = 16.0/static_cast<float>(area);
        edge e0, e1, e2;

        int32x4_t w0 = e0.init(p1, p0, minV);
        int32x4_t w1 = e1.init(p2, p1, minV);
        int32x4_t w2 = e2.init(p0, p2, minV);



        for (int32_t y = minV[1]; y <= (maxV[1]); y += (edge::stepYsize)) {
            int32x4_t w0_temp = w0;
            int32x4_t w1_temp = w1;
            int32x4_t w2_temp = w2;
            for (int32_t x = minV[0]; x <= (maxV[0]); x += (edge::stepXsize)) {
                
                int32x4_t mask = vorrq_s32(w0_temp, w1_temp);
                mask = vorrq_s32(mask, w2_temp);
                
                //mask = vcgeq_s32(mask, );
                //need to now check if any of the masks are positive?
                //whats the best way?
                //if I can reduce the whole vector to a single scalar that could work?
                int32_t mask_min = vmaxvq_s32(mask);
                //std::cout << mask_min << "\n";
                if (mask_min >= 0) {
                    drawPixel(x, y, w0_temp, w1_temp, w2_temp, mask, face, v0, v1, v2, oneOnArea, m);
                }
                w0_temp = vaddq_s32(w0_temp, e0.oneStepX);
                w1_temp = vaddq_s32(w1_temp, e1.oneStepX);
                w2_temp = vaddq_s32(w2_temp, e2.oneStepX);
            }
            w0 = vaddq_s32(w0, e0.oneStepY);
            w1 = vaddq_s32(w1, e1.oneStepY);
            w2 = vaddq_s32(w2, e2.oneStepY);
        }
    };


    void rasterizer::drawPixel(int32_t x, int32_t y, int32x4_t const &w0, int32x4_t const &w1, int32x4_t const &w2, int32x4_t const &mask, Face const &f, vec4 const &v0, vec4 const &v1, vec4 const &v2, float const &oneOnArea, model const &m){
        //Probably convert all of this to a DDA
        float v0w = 1/v0.w * oneOnArea;
        float v1w = 1/v1.w * oneOnArea;
        float v2w = 1/v2.w * oneOnArea;
        float v0z = v0.z * v0w;
        float v1z = v1.z * v1w;
        float v2z = v2.z * v2w;
        float v0u = (m.vertexUVs[f.UVs[0]].x) * v0w;
        float v1u = (m.vertexUVs[f.UVs[1]].x) * v1w;
        float v2u = (m.vertexUVs[f.UVs[2]].x) * v2w;
        float v0v = (1 - (m.vertexUVs[f.UVs[0]].y)) * v0w;
        float v1v = (1 - (m.vertexUVs[f.UVs[1]].y)) * v1w;
        float v2v = (1 - (m.vertexUVs[f.UVs[2]].y)) * v2w;

        int normalX = (x) >> 4;
        int normalY = (y) >> 4;

        //change these to more efficient sign checks later maybe
        if (vgetq_lane_s32(mask, 0) >= 0) {
            float W0 = static_cast<float>(vgetq_lane_s32(w1, 0))/16 * v0w + static_cast<float>(vgetq_lane_s32(w2, 0))/16 * v1w + static_cast<float>(vgetq_lane_s32(w0, 0))/16 * v2w;
            W0 = 1/W0;
            float z0 = static_cast<float>(vgetq_lane_s32(w1, 0))/16 * v0z + static_cast<float>(vgetq_lane_s32(w2, 0))/16 * v1z + static_cast<float>(vgetq_lane_s32(w0, 0))/16 * v2z;
            z0 *= W0;
            float u0 = static_cast<float>(vgetq_lane_s32(w1, 0))/16 * v0u + static_cast<float>(vgetq_lane_s32(w2, 0))/16 * v1u + static_cast<float>(vgetq_lane_s32(w0, 0))/16 * v2u;
            u0 *= W0;
            float V0 = static_cast<float>(vgetq_lane_s32(w1, 0))/16 * v0v + static_cast<float>(vgetq_lane_s32(w2, 0))/16 * v1v + static_cast<float>(vgetq_lane_s32(w0, 0))/16 * v2v;
            V0 *= W0;

            int tex0u = static_cast<int>((u0) * 1024);
            int tex0v = static_cast<int>((V0) * 1024) * 1024;
            int colour0 = static_cast<int>((1 - z0) * 255);
            if (z0 >= 0 && z0 <= 1 && this->depthBuffer[normalX + (normalY*this->width)] > z0) {
                this->colorBuffer[normalX + (normalY*this->width)] = m.albedo[tex0u + tex0v];
                // this->colorBuffer[normalX + (normalY*this->width)] = 0xff000000 | colour0 | (colour0 << 8) | (colour0 << 16);  
                // this->colorBuffer[normalX + ((normalY)*this->width)] =  m.albedo[((normalX)/this->height) * 1024 + (((normalY)/this->width) * 1024 * 1024)];
                this->depthBuffer[normalX + (normalY*this->width)] = z0;
            }
        }

        if (normalX + 1 <= this->width && vgetq_lane_s32(mask, 1) >= 0) {
            float W1 = static_cast<float>(vgetq_lane_s32(w1, 1))/16 * v0w + static_cast<float>(vgetq_lane_s32(w2, 1))/16 * v1w + static_cast<float>(vgetq_lane_s32(w0, 1))/16 * v2w;
            W1 = 1/W1;
            float z1 = static_cast<float>(vgetq_lane_s32(w1, 1))/16 * v0z + static_cast<float>(vgetq_lane_s32(w2, 1))/16 * v1z + static_cast<float>(vgetq_lane_s32(w0, 1))/16 * v2z;
            z1 *= W1;
            float u1 = static_cast<float>(vgetq_lane_s32(w1, 1))/16 * v0u + static_cast<float>(vgetq_lane_s32(w2, 1))/16 * v1u + static_cast<float>(vgetq_lane_s32(w0, 1))/16 * v2u;
            u1 *= W1;
            float V1 = static_cast<float>(vgetq_lane_s32(w1, 1))/16 * v0v + static_cast<float>(vgetq_lane_s32(w2, 1))/16 * v1v + static_cast<float>(vgetq_lane_s32(w0, 1))/16 * v2v;
            V1 *= W1;

            int tex1u = static_cast<int>(u1 * 1024);
            int tex1v = static_cast<int>((V1) * 1024) * 1024;
            int colour1 = static_cast<int>((1 - z1) * 255);
            if (z1 >= 0 && z1 <= 1 && this->depthBuffer[normalX + 1 + (normalY*this->width)] > z1){
                this->colorBuffer[normalX + 1 + (normalY*this->width)] = m.albedo[tex1u + tex1v];
                // this->colorBuffer[normalX + 1 + (normalY*this->width)] = 0xff000000 | colour1 | (colour1 << 8) | (colour1 << 16);
                // this->colorBuffer[normalX + 1 + ((normalY)*this->width)] =  m.albedo[((normalX + 1)/this->height) * 1024 + (((normalY)/this->width) * 1024 * 1024)];
                this->depthBuffer[normalX + 1 + (normalY*this->width)] = z1;
            }
        }

        if (normalY + 1 <= this->height && vgetq_lane_s32(mask, 2) >= 0) {
            float W2 = static_cast<float>(vgetq_lane_s32(w1, 2))/16 * v0w + static_cast<float>(vgetq_lane_s32(w2, 2))/16 * v1w + static_cast<float>(vgetq_lane_s32(w0, 2))/16 * v2w;
            W2 = 1/W2;
            float z2 = static_cast<float>(vgetq_lane_s32(w1, 2))/16 * v0z + static_cast<float>(vgetq_lane_s32(w2, 2))/16 * v1z + static_cast<float>(vgetq_lane_s32(w0, 2))/16 * v2z;
            z2 *= W2;
            float u2 = static_cast<float>(vgetq_lane_s32(w1, 2))/16 * v0u + static_cast<float>(vgetq_lane_s32(w2, 2))/16 * v1u + static_cast<float>(vgetq_lane_s32(w0, 2))/16 * v2u;
            u2 *= W2;
            float V2 = static_cast<float>(vgetq_lane_s32(w1, 2))/16 * v0v + static_cast<float>(vgetq_lane_s32(w2, 2))/16 * v1v + static_cast<float>(vgetq_lane_s32(w0, 2))/16 * v2v;
            V2 *= W2;

            int tex2u = static_cast<int>(u2 * 1024);
            int tex2v = static_cast<int>((V2) * 1024) * 1024;
            int colour2 = static_cast<int>((1 - z2) * 255);
            if (z2 >= 0 && z2 <= 1 && this->depthBuffer[normalX + ((normalY + 1)*this->width)] > z2) {
                this->colorBuffer[normalX + ((normalY + 1)*this->width)] = m.albedo[tex2u + tex2v];
                // this->colorBuffer[normalX + ((normalY + 1)*this->width)] = 0xff000000 | colour2 | (colour2 << 8) | (colour2 << 16);
                // this->colorBuffer[normalX + ((normalY + 1)*this->width)] =  m.albedo[((normalX)/this->height) * 1024 + (((normalY + 1)/this->width) * 1024 * 1024)];
                this->depthBuffer[normalX + ((normalY + 1)*this->width)] = z2;
            }        
        }

        if (normalX + 1 <= this->width && normalY + 1 <= this->height && vgetq_lane_s32(mask, 3) >= 0) {
            float W3 = static_cast<float>(vgetq_lane_s32(w1, 3))/16 * v0w + static_cast<float>(vgetq_lane_s32(w2, 3))/16 * v1w + static_cast<float>(vgetq_lane_s32(w0, 3))/16 * v2w;
            W3 = 1/W3;
            float z3 = static_cast<float>(vgetq_lane_s32(w1, 3))/16 * v0z + static_cast<float>(vgetq_lane_s32(w2, 3))/16 * v1z + static_cast<float>(vgetq_lane_s32(w0, 3))/16 * v2z;
            z3 *= W3;
            float u3 = static_cast<float>(vgetq_lane_s32(w1, 3))/16 * v0u + static_cast<float>(vgetq_lane_s32(w2, 3))/16 * v1u + static_cast<float>(vgetq_lane_s32(w0, 3))/16 * v2u;
            u3 *= W3;
            float V3 = static_cast<float>(vgetq_lane_s32(w1, 3))/16 * v0v + static_cast<float>(vgetq_lane_s32(w2, 3))/16 * v1v + static_cast<float>(vgetq_lane_s32(w0, 3))/16 * v2v;
            V3 *= W3;

            int tex3u = static_cast<int>(u3 * 1024);
            int tex3v = static_cast<int>((V3) * 1024) * 1024;
            int colour3 = static_cast<int>((1 - z3) * 255);
            if (z3 >= 0 && z3 <= 1 && this->depthBuffer[normalX + 1 + ((normalY + 1)*this->width)] > z3) {
                this->colorBuffer[normalX + 1 + ((normalY + 1)*this->width)] =  m.albedo[tex3u + tex3v];
                // this->colorBuffer[normalX + 1 + ((normalY + 1)*this->width)] =  0xff000000 | colour3 | (colour3 << 8) | (colour3 << 16);
                // this->colorBuffer[normalX + 1 + ((normalY + 1)*this->width)] =  m.albedo[((normalX + 1)/this->height) * 1024 + (((normalY + 1)/this->width) * 1024 * 1024)];
                this->depthBuffer[normalX + 1 + ((normalY + 1)*this->width)] = z3;
            }   
        }
        // this->colorBuffer[static_cast<int>(v0.x) + (static_cast<int>(v0.y) * this->width)] = m.albedo[(static_cast<int>(m.vertexUVs[f.UVs[0]].x) * 1024) + ((static_cast<int>(m.vertexUVs[f.UVs[0]].y) * 1024) * 1024)];
        
        // this->colorBuffer[static_cast<int>(v1.x) + (static_cast<int>(v1.y) * this->width)] = m.albedo[(static_cast<int>(m.vertexUVs[f.UVs[1]].x) * 1024) + ((static_cast<int>(m.vertexUVs[f.UVs[1]].y) * 1024) * 1024)];
        
        // this->colorBuffer[static_cast<int>(v2.x) + (static_cast<int>(v2.y) * this->width)] = m.albedo[(static_cast<int>(m.vertexUVs[f.UVs[2]].x) * 1024) + ((static_cast<int>(m.vertexUVs[f.UVs[2]].y) * 1024) * 1024)];

        //for now just write the depth buffer to screen
    }

}