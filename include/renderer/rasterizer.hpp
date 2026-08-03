#pragma once
#include <renderer/typeDefs.hpp>

namespace renderer{
    class rasterizer {
        public:
        std::vector<uint32_t> rasterize(renderer::scene scene, int height, int width);

        rasterizer() = default;

        private:
        std::vector<uint32_t> colorBuffer;
        std::vector<float> depthBuffer;
        void drawTri(vec3 const &v0, vec3 const &v1, vec3 const &v2);
        void drawPixel(vec2 p, int w0, int w1, int w2, Face f);
    };
    std::vector<uint32_t> rasterize(scene scene, int height, int width);

}