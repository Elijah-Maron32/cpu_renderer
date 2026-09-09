#include <renderer/rasterizer.hpp>
#include <renderer/math.hpp>
#include <renderer/typeDefs.hpp>
#include <iostream>
#include <limits>


namespace renderer {


    std::vector<uint32_t> rasterizer::rasterize(renderer::scene const &scene, int32_t height, int32_t width) {
        if (this->height != height && this->width!=width) {
            this->colorBuffer.resize(height*width);
            this->depthBuffer.resize(height*width);
            this->height = height;
            this->width = width;
            aspectRatio = static_cast<float>(width)/height;
        }

        //clear buffers
        std::fill_n(this->colorBuffer.begin(), height*width, 0xFFFFFFFF);
        std::fill_n(this->depthBuffer.begin(), height*width, std::numeric_limits<float>::infinity());        
        
        for(renderer::model object : scene.models) {
            //--Vertex shader--
            //Local space to world space
            renderer::AlignedVec4 worldVerts = renderer::localToTransform(object.transform, object.vertexPositions);
            //World space to camera space to clip space (NDC)
            renderer::AlignedVec4 projectedVerts = renderer::getProjectedCoordinates(
                worldVerts, scene.cameraPos, scene.fov, scene.near, scene.far, aspectRatio);
            //Clip space to raster co-ordinates
            renderer::AlignedVec4 rasterVerts = renderer::getRasterCoords(projectedVerts, height, width);
            for(renderer::Face face : object.faceVerticies) {
                drawTri(rasterVerts[face.Verts[0]], rasterVerts[face.Verts[1]], rasterVerts[face.Verts[2]], face, object);
            }
        }

        return this->colorBuffer;
    }
    
    struct edge {
        //32 because of the fixed point values
        static const int32_t stepXsize = 32;
        static const int32_t stepYsize = 32;

        //derivatives
        int32x4_t oneStepX;
        int32x4_t oneStepY;

        

        int32x4_t init(std::array<int32_t,2> v0, std::array<int32_t,2> v1, std::array<int32_t,2> origin);
    };

    int32x4_t edge::init(std::array<int32_t,2> v0,std::array<int32_t,2> v1, std::array<int32_t,2> origin) {
        //This is a way of rearranging the edge function in terms of x and y in screen space
        //a is the partial derivative with regards to x
        //b is the partial derivative with regards to y
        //c is our constant term
        //If you want a good explanation of the math check the references in the readme for 'The Barycentric conspiracy"
        int32_t a = v0[1] - v1[1];
        int32_t b = v1[0] - v0[0];
        int32_t c = (((v0[0]*v1[1]) + 8) >> 4) - (((v0[1]*v1[0]) + 8) >> 4);

        //We can do a << 1 since our step size is 2 so it's just the same as * 2 
        oneStepX = vmovq_n_s32(a << 1);
        oneStepY = vmovq_n_s32(b << 1);
        
        //calculate the value of our edge function for out initial grid of four pixels
        //so just A*x + B*y + C for each pixel
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
    
    int computeArea(std::array<int,2> const v0, std::array<int,2> const v1, std::array<int,2> const v2) {
        return (((((v1[0] - v0[0]) * (v2[1] - v0[1])) + 8) >> 4) - ((((v1[1] - v0[1]) * (v2[0] - v0[0])) + 8 )>> 4));
    }

    struct triConstants {
        float v0w;
        float v1w;
        float v2w;
        float v0z;
        float v1z;
        float v2z;
        float v0u;
        float v1u;
        float v2u;
        float v0v;
        float v1v;
        float v2v;

        void calculateConstants(vec4 const v0, vec4 const v1, vec4 const v2, model const &m, Face const f, float oneOnArea) {
            //We prefactor in 1/area to avoid factoring them in later at every pixel when we interpolate
            //the area gets canceled out by the fact we don't normalise our barycentric weights during interpolation
            //everything is interpolated in terms of 1/w
            //(which is the value of z in camera space) because that is actually
            //a linear relationship in screen space because of the perpective transformation
            v0w = 1/v0.w * oneOnArea;
            v1w = 1/v1.w * oneOnArea;
            v2w = 1/v2.w * oneOnArea;
            v0z = v0.z * v0w;
            v1z = v1.z * v1w;
            v2z = v2.z * v2w;
            v0u = (m.vertexUVs[f.UVs[0]].x) * v0w;
            v1u = (m.vertexUVs[f.UVs[1]].x) * v1w;
            v2u = (m.vertexUVs[f.UVs[2]].x) * v2w;
            v0v = (1 - (m.vertexUVs[f.UVs[0]].y)) * v0w;
            v1v = (1 - (m.vertexUVs[f.UVs[1]].y)) * v1w;
            v2v = (1 - (m.vertexUVs[f.UVs[2]].y)) * v2w;
        }
    };

    
    void rasterizer::drawTri(vec4 const v0, vec4 const v1, vec4 const v2, Face const face, model const &m) {
 
        //This is a conversion from float to fixed point
        //The fixed point ints use 4 fractional bits
        //loading values into registers
        //we only load the first two elements of our vectors (x and y)
        float32x2_t scaling = vmov_n_f32(16);
        float32x2_t rounding = vmov_n_f32(0.5);
        float32x2_t V0 = vld1_f32(reinterpret_cast<float const *>(&v0));
        float32x2_t V1 = vld1_f32(reinterpret_cast<float const *>(&v1));
        float32x2_t V2 = vld1_f32(reinterpret_cast<float const *>(&v2));
       
        //multiply by 16 to approximate << 4
        V0 = vmul_f32(V0, scaling);
        V1 = vmul_f32(V1, scaling);
        V2 = vmul_f32(V2, scaling);

        //adds a rounding factor
        V0 = vadd_f32(V0, rounding);
        V1 = vadd_f32(V1, rounding);
        V2 = vadd_f32(V2, rounding);
        
        //convert to int
        int32x2_t P0 = vcvtm_s32_f32(V0);
        int32x2_t P1 = vcvtm_s32_f32(V1);
        int32x2_t P2 = vcvtm_s32_f32(V2);
        
        //storing into usable arrays
        std::array<int32_t,2> p0;
        vst1_s32(p0.data(), P0);
        std::array<int32_t,2> p1;
        vst1_s32(p1.data(), P1);
        std::array<int32_t,2> p2;
        vst1_s32(p2.data(), P2);
        
        //Construction our bounding box
        std::array<int32_t,2> minV;
        int32x2_t zero = vmov_n_s32(0);
        int32x2_t min = vmin_s32(P0, P1);
        min = vmin_s32(min, P2);
        //ensuring we don't actually try to draw pixels off screen by clipping the bounding box to zero
        min = vmax_s32(min, zero);
        vst1_s32(minV.data(), min);
        
        std::array<int32_t,2> maxV;
        int32x2_t max = vmax_s32(P0, P1);
        int32x2_t screen = {((this->width - 1) << 4), ((this->height - 1) << 4)};
        max = vmax_s32(max, P2);
        //ensuring we don't try to draw pixels off screen by clipping to screen size
        max = vmin_s32(max, screen);
        vst1_s32(maxV.data(), max);

        //this is effectivley a round to ceiling (As far as I know this should also kind of approximate a top left fill rule but I need to double check)
        minV[0] = ((minV[0] + 15)  & (~15));
        minV[1] = ((minV[1] + 15)  & (~15));
        
        //Check for degenerate triangles (kind of also back face culling since this is a signed area caluclation)
        int area = computeArea(p2, p1, p0);
        if (area < 0) {
            return;
        }

        //precompute inverse area for later
        float oneOnArea = 16.0/static_cast<float>(area);
        edge e0, e1, e2;

        //precompute interpolation constants
        triConstants c;
        c.calculateConstants(v0, v1, v2, m, face, oneOnArea);

        //create our initial barycentric weights. (Calculate edge function at starting point)
        int32x4_t w0 = e0.init(p1, p0, minV);
        int32x4_t w1 = e1.init(p2, p1, minV);
        int32x4_t w2 = e2.init(p0, p2, minV);

        
        //stride along bounding box with a 2x2 grid of pixels
        for (int32_t y = minV[1]; y <= (maxV[1]); y += (edge::stepYsize)) {
            //Create a temporary copy of our weights so we can move along x then come back to the start later
            int32x4_t w0_temp = w0;
            int32x4_t w1_temp = w1;
            int32x4_t w2_temp = w2;
            for (int32_t x = minV[0]; x <= (maxV[0]); x += (edge::stepXsize)) {
                
                //calculate a coverage mask based on edge function outputs
                int32x4_t mask = vorrq_s32(w0_temp, w1_temp);
                mask = vorrq_s32(mask, w2_temp);
                
                uint32x4_t bitMask = vcgeq_s32(mask, vmovq_n_s32(0));
                //if the max value is negative we know nothing is in the traingle and can stop early
                int32_t mask_max = vmaxvq_s32(mask);
                if (mask_max >= 0) {
                    drawPixel(x, y, w0_temp, w1_temp, w2_temp, bitMask, m, c);
                }
                //interpolate edge function values one pixel to the right
                w0_temp = vaddq_s32(w0_temp, e0.oneStepX);
                w1_temp = vaddq_s32(w1_temp, e1.oneStepX);
                w2_temp = vaddq_s32(w2_temp, e2.oneStepX);
            }
            //interpolate edge function values one pixel down
            w0 = vaddq_s32(w0, e0.oneStepY);
            w1 = vaddq_s32(w1, e1.oneStepY);
            w2 = vaddq_s32(w2, e2.oneStepY);
        }
    };


    void rasterizer::drawPixel(int32_t x, int32_t y, int32x4_t const w0, int32x4_t const w1, int32x4_t const w2, uint32x4_t const mask, model const &m, triConstants const c){

        //converts back from fixed point
        int normalX = (x) >> 4;
        int normalY = (y) >> 4;

        //This is just creating an updated bitmask that cuts off pixels that are off-screen
        int32x4_t xTest = {0,1,0,1};
        xTest = vaddq_s32(xTest, vmovq_n_s32(normalX));
        uint32x4_t xMask = vcltq_s32(xTest, vmovq_n_s32(width));
        
        int32x4_t yTest = {0,0,1,1};
        yTest = vaddq_s32(yTest, vmovq_n_s32(normalY));
        uint32x4_t yMask = vcltq_s32(yTest, vmovq_n_s32(height));
        
        uint32x4_t bitMask = vandq_s32(xMask, yMask);
        bitMask = vandq_s32(bitMask, mask);
        
        //Load our edge function weights and convert them back from fixed point (divide by 16)
        //Could precompute an aproximate value for 1/16?
        float32x4_t sixteen = vmovq_n_f32(1.0/16.0);
        float32x4_t L0 = vcvtq_f32_s32(w0);
        L0 = vmulq_f32(L0, sixteen);
        float32x4_t L1 = vcvtq_f32_s32(w1);
        L1 = vmulq_f32(L1, sixteen);
        float32x4_t L2 = vcvtq_f32_s32(w2);
        L2 = vmulq_f32(L2, sixteen);

        //load vertex 1/W * 1/area
        float32x4_t v0W = vmovq_n_f32(c.v0w);
        float32x4_t v1W = vmovq_n_f32(c.v1w);
        float32x4_t v2W = vmovq_n_f32(c.v2w);

        //load vertex z/w
        float32x4_t v0Z = vmovq_n_f32(c.v0z);
        float32x4_t v1Z = vmovq_n_f32(c.v1z);
        float32x4_t v2Z = vmovq_n_f32(c.v2z);

        //load vertex u/w
        float32x4_t v0U = vmovq_n_f32(c.v0u);
        float32x4_t v1U = vmovq_n_f32(c.v1u);
        float32x4_t v2U = vmovq_n_f32(c.v2u);

        //load vertex v/w
        float32x4_t v0V = vmovq_n_f32(c.v0v);
        float32x4_t v1V = vmovq_n_f32(c.v1v);
        float32x4_t v2V = vmovq_n_f32(c.v2v);

        //calculate pixel 1/w
        //1/w = l0 * v2w + l1 * v0w + l2 * v1w
        float32x4_t pWrec = vmovq_n_f32(0);
        pWrec = vfmaq_f32(pWrec, L0, v2W);
        pWrec = vfmaq_f32(pWrec, L1, v0W);
        pWrec = vfmaq_f32(pWrec, L2, v1W);

        //calculate pixel w
        //using newton-raphson approximation to avoid division
        //Built in reciprocal estimation ~8 bits precision
        float32x4_t pW = vrecpeq_f32(pWrec);
        //calculating 1/d ~ x(n+1) = x(n)(2-(x(n)*d))
        //this improves our approximation to ~16 bits precision
        float32x4_t pWstep = vrecpsq_f32(pWrec, pW);
        pW = vmulq_f32(pW, pWstep);
        //doing the step again gives us ~32 bits precisions, we're only working with 32 bits, so no need to do more
        pWstep = vrecpsq_f32(pWrec, pW);
        pW = vmulq_f32(pW, pWstep);

        //calculate pixel z/w
        float32x4_t pZ = vmovq_n_f32(0);
        pZ = vfmaq_f32(pZ, L0,v2Z);
        pZ = vfmaq_f32(pZ, L1,v0Z);
        pZ = vfmaq_f32(pZ, L2,v1Z);
        //Factor out w
        pZ = vmulq_f32(pZ, pW);

        //Load depth buffer and computed z for the top two pixels
        float32x2_t depth1 = vld1_f32(depthBuffer.data() + (normalX + (width * normalY)));
        float32x2_t z1 = vget_low_f32(pZ);
        uint32x2_t depthTest1 = vcle_f32(z1, depth1);
        depthTest1 = vand_u32(vget_low_u32(bitMask), depthTest1);
        //selectivley replace depth buffer value with new correct value and store
        depth1 = vbsl_f32(depthTest1, z1, depth1);
        vst1_f32(depthBuffer.data() + (normalX + (width * normalY)), depth1);

        //Same process as above but for the bottom two pixels
        float32x2_t depth2 = vld1_f32(depthBuffer.data() + (normalX + (width * (normalY + 1))));
        float32x2_t z2 = vget_high_f32(pZ);
        uint32x2_t depthTest2 = vcle_f32(z2, depth2);
        depthTest2 = vand_u32(depthTest2, vget_high_u32(bitMask));
        depth2 = vbsl_f32(depthTest2, z2, depth2);
        vst1_f32(depthBuffer.data() + (normalX + (width * (normalY + 1))), depth2);

        //early out if all pixels are covered
        if (vget_lane_u32(depthTest1, 0) == 0
        && vget_lane_u32(depthTest1, 1) == 0
        && vget_lane_u32(depthTest2, 0) == 0
        && vget_lane_u32(depthTest2, 1) == 0) {
            return;
        }

        //calculate pixel u/w
        float32x4_t pU = vmovq_n_f32(0);
        pU = vfmaq_f32(pU, L0,v2U);
        pU = vfmaq_f32(pU, L1,v0U);
        pU = vfmaq_f32(pU, L2,v1U);
        //Factor out w
        pU = vmulq_f32(pU, pW);
        //get actual on texture co-ordinate value
        float32x4_t texWidth = vmovq_n_f32(1024);
        pU = vmulq_f32(pU, texWidth);
        int32x4_t texU = vcvtq_s32_f32(pU);

        //calculate pixel v/w
        float32x4_t pV = vmovq_n_f32(0);
        pV = vfmaq_f32(pV, L0,v2V);
        pV = vfmaq_f32(pV, L1,v0V);
        pV = vfmaq_f32(pV, L2,v1V);
        //Factor out w
        pV = vmulq_f32(pV, pW);
        //get actual on texture co-ordinate value
        float32x4_t texHeight = vmovq_n_f32(1024);
        pV = vmulq_f32(pV, texHeight);
        int32x4_t texV = vcvtq_s32_f32(pV);

        //if we wanted to do our colour writes the same way we do our depth writes,
        //we would need to also load the old values,
        //which is unneccessary here but was a required step for depth anyway
        //I also decided against vectorising this step since doing the array reads with neon seemed unnecessarily
        //complicated since gather loads aren't a thing on mac so I would basically need to do scalar code anyway
        if (vget_lane_u32(depthTest1, 0) != 0) {
            colorBuffer[normalX + (normalY*width)] = m.albedo[vgetq_lane_s32(texU, 0) + (vgetq_lane_s32(texV, 0) * 1024)];

        }

        if (vget_lane_u32(depthTest1, 1) != 0) {
            colorBuffer[(normalX + 1) + (normalY*width)] = m.albedo[vgetq_lane_s32(texU, 1) + (vgetq_lane_s32(texV, 1) * 1024)];
        }

        if (vget_lane_u32(depthTest2, 0) != 0) {
            colorBuffer[normalX + ((normalY + 1)*width)] = m.albedo[vgetq_lane_s32(texU, 2) + (vgetq_lane_s32(texV, 2) * 1024)];
        }

        if (vget_lane_u32(depthTest2, 1) != 0) {
            colorBuffer[(normalX + 1) + ((normalY + 1)*width)] = m.albedo[vgetq_lane_s32(texU, 3) + (vgetq_lane_s32(texV, 3) * 1024)];
        }
    }

}