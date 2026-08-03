#include <renderer/rasterizer.hpp>
#include <renderer/math.hpp>
#include <renderer/typeDefs.hpp>
#include <iostream>
#include <arm_neon.h>
#include <limits>

namespace renderer {


    std::vector<uint32_t> rasterizer::rasterize(renderer::scene scene, int height, int width) {
        this->colorBuffer.resize(height*width);
        this->depthBuffer.resize(height*width);
        std::fill_n(this->colorBuffer.begin(), height*width, 0xFFFFFFFF);
        std::fill_n(this->depthBuffer.begin(), height*width, std::numeric_limits<float>::infinity());
        
        float aspectRatio = static_cast<float>(width)/height;

        for(renderer::model object : scene.models) {

            renderer::AlignedVec4 worldVerts = renderer::localToTransform(object.transform, object.vertexPositions);
            renderer::AlignedVec4 projectedVerts = renderer::getProjectedCoordinates(
                worldVerts, scene.cameraPos, scene.fov, scene.near, scene.far, aspectRatio);
            
            renderer::AlignedVec3 rasterVerts = renderer::getRasterCoords(projectedVerts, height, width);
            
            for(renderer::Face face : object.faceVerticies) {
                drawTri(rasterVerts[face.Verts[0]], rasterVerts[face.Verts[1]], rasterVerts[face.Verts[2]]);
            }

            int i = 0;
            for (renderer::vec3 point: rasterVerts) {
                int x = static_cast<int>(point.x);
                int y = static_cast<int>(point.y);
                //std::cout << i << "," << x << "," << y << "," << point.z << "\n";

                if (x >= width || x < 0 || y >= height || y < 0) {
                    //std::cout << i << "," << x << "," << y << "\n";
                    continue;
                }

                if (depthBuffer[(x) + (y*height)] > point.z) {
                    //std::cout << i << "," << x << "," << y << "\n";
                    depthBuffer[x + (y*width)] = point.z;
                    colorBuffer[x + (y*width)] = 0xFF000000;

                    // colorBuffer[x+1 + (y*width)] = 0xFF000000;
                    // colorBuffer[x-1 + (y*width)] = 0xFF000000;
                    // colorBuffer[x+1 + ((y+1)*width)] = 0xFF000000;
                    // colorBuffer[x+1 + ((y-1)*width)] = 0xFF000000;

                }
                ++i;
            }
        }

        return colorBuffer;
    }

    //is there a way to do a simd bounding box with only 3 verticies?
    //if we just load our verts in AoS then doing two max and min calls could work?
    //but we're only really doing 2 sets at once
    //okay lets try that
    void rasterizer::drawTri(vec3 const &v0, vec3 const &v1, vec3 const &v2) {
        
        float32x2_t V0 = vld1_f32(reinterpret_cast<float const *>(&v0));
        float32x2_t V1 = vld1_f32(reinterpret_cast<float const *>(&v1));
        float32x2_t V2 = vld1_f32(reinterpret_cast<float const *>(&v2));

        vec2 minV;
        float32x2_t zero = vmov_n_f32(0);
        float32x2_t min = vmin_f32(V0, V1);
        min = vmin_f32(min, V2);
        min = vmin_f32(min, zero);
        vst1_f32(reinterpret_cast<float *>(&minV), min);

        vec2 maxV;
        float32x2_t max = vmax_f32(V0, V1);
        float32x2_t screen = {1280, 720};
        max = vmax_f32(min, V2);
        vst1_f32(reinterpret_cast<float *>(&maxV), max);

        int const minX = static_cast<int>(minV.x);
        int const minY = static_cast<int>(minV.y);
        int const maxX = static_cast<int>(maxV.x);
        int const maxY = static_cast<int>(maxV.y);

    };

    void rasterizer::drawPixel(vec2 p, int w0, int w1, int w2, Face f) {

    }


}