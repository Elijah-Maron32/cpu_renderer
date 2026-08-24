#pragma once
#include <renderer/typeDefs.hpp>
#include <arm_neon.h>

namespace renderer{
    struct triConstants;
    
    class rasterizer {
        public:
        std::vector<uint32_t> rasterize(renderer::scene scene, int32_t height, int32_t width);

        rasterizer() = default;

        private:
        std::vector<uint32_t> colorBuffer;
        std::vector<float> depthBuffer;
        int32_t height;
        int32_t width;
        void drawTri(vec4 const &v0, vec4 const &v1, vec4 const &v2, Face const &face, model const &m);
        void drawPixel(int32_t x, int32_t y, int32x4_t const &w0, int32x4_t const &w1, int32x4_t const &w2, uint32x4_t const &mask, model const &m, triConstants const &c);
    };
    //std::vector<uint32_t> rasterize(scene scene, int32_t height, int32_t width);
    
}