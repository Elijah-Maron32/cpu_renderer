#pragma once
#include <renderer/typeDefs.hpp>
#include <arm_neon.h>

namespace renderer{
    struct triConstants;
    
    class rasterizer {
        public:
        /* rasterize
            This is the function that invokes the whole render pipeline

            Arguments:
            scene: the list of all models, the camera position, etc.
            height, width: target resolution dimensions
        */
        std::vector<uint32_t> rasterize(renderer::scene const &scene, int32_t height, int32_t width);

        rasterizer() = default;

        private:
        //buffer containing all pixel colours to draw to the screen
        std::vector<uint32_t> colorBuffer;
        //buffer containing the depth value of the closest object to the camera at each pixel
        std::vector<float> depthBuffer;
        //target resolution height
        int32_t height;
        //target resolution width
        int32_t width;
        //
        float aspectRatio;

        /* drawTri
            This function is the section of the rasterizer that does triangle setup,
            as well as edge function tests.
            *Could theoretically be improved by having it do the setup for 4 triangles at once in a future update.
            
            arguments:
            v0,v1,v2: position vectors for each vertex
            face: the face struct that defines the windings and indexes in the models arrays of vertexes and UVS
            m: model struct for the triangles model
        */
        void drawTri(vec4 const v0, vec4 const v1, vec4 const v2, Face const face, model const &m);
        /* drawPixel
            This is the pretty much fragement shader, though it does do depth testing as well. It calculates interpolated vertex attriburtes for the pixel.
            Performs depth testing and vertex attribute interpolation. Performs an albedo texture lookup and draws the value to the screen.
            Calculates for a 2x2 grid of pixels, Unfortunatley there isn't really a way to do conditional or scatter stores with ARM so we still have to do scalar stores.

            Arguments:
            X, Y: fixed point (4 fractional bits) positions in rasterspace for the pixel
            w0, w1, w2: Barycentric weight vectors for the pixels stored a fixed points
            mask: coverage mask for the quad
            m: model struct to be able to grab UVs and perform texture lookup
            c: a struct containing precomputed triangle constants for triangle interpolation
        */
        void drawPixel(int32_t x, int32_t y, int32x4_t const w0, int32x4_t const w1, int32x4_t const w2, uint32x4_t const mask, model const &m, triConstants const c);
    };
    
}