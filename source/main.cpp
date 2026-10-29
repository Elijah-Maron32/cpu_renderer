// #define TRACY_ENABLE
#include <iostream>
#include "SDL.h"
#include <chrono>
#include <renderer/modelImporter.hpp>
#include <renderer/math.hpp>
#include <cmath>
#include <numbers>
#include <renderer/rasterizer.hpp>
#include <tracy/Tracy.hpp>


#define FPS 60.0
#define TARGET_DT 1.0 / FPS

using namespace renderer;

int main(int argc, char* args[]){
    #ifdef TRACY_ENABLE
        std::cout << "tracy running";
    #endif
    vec4 cameraPos = {0,1,1,1};    
    vec4 cameraVel = {0,0,0,0};
    float const fov = 90, near = 1, far = 5;

    model m;
    model m1;

    parsePNG("necoarctexture.png", m.albedo);
    parsePNG("cubeTexture.png", m1.albedo);

    parseOBJFile("necoarc.obj", m.vertexPositions, m.faceVerticies, m.vertexUVs);
    parseOBJFile("cube.obj", m1.vertexPositions, m1.faceVerticies, m1.vertexUVs);

    m.transform = {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    };

    m1.transform = {
        0.5, 0, 0, 0,
        0, 2, 0, 0,
        0, 0, 0.5, 0,
        0, 0, 0, 1
    };

    vec4 translation = {0, 0, -2.5, 1};
    vec4 translation1 = {0, 1, -2.5, 1};


    //TODO: potential change if ditching pure functional constraints
    m.transform = applyTranslation(m.transform, translation);
    
    m1.transform = applyRotationY(m1.transform, std::numbers::pi/4);
    m1.transform = applyTranslation(m1.transform, translation1);



    //TODO: Consider a debug ifdef for logging
    //std::cout << "output\n";
    
    scene stuff;
    rasterizer raster;
    stuff.cameraPos = cameraPos;
    stuff.fov = fov;
    stuff.far = far;
    stuff.near = near;
    stuff.models = {m};
    
    int width = 1280;
    int height = 720;
    

    
    SDL_Window* window = SDL_CreateWindow("ARM 3D Renderer",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        width, height,
        SDL_WINDOW_SHOWN);

    SDL_Surface * draw_surface = nullptr;

    int mouse_x = 0;
    int mouse_y = 0;

    using clock = std::chrono::high_resolution_clock;
    auto prev_frame = clock::now();
    auto runtime = clock::now();


    bool running = true;
    while (running)
    {
        for (SDL_Event event; SDL_PollEvent(&event);) switch (event.type)
        {
            case SDL_QUIT:
                running = false;
                break;
            case SDL_MOUSEMOTION:
                mouse_x = event.motion.x;
                mouse_y = event.motion.y;
                break;
            
            // TODO: change this code so that movement is more stable
            // case SDL_KEYDOWN:
            //     switch(event.key.keysym.sym) {
            //         case SDLK_w:
            //             cameraVel += vec4{0, 0, 1, 0};
            //             break;
            //         case SDLK_s:
            //             cameraVel += vec4{0, 0, -1, 0};
            //             break;
            //         case SDLK_a:
            //             cameraVel += vec4{-1, 0, 0, 0};
            //             break;
            //         case SDLK_d:
            //             cameraVel += vec4{1, 0, 0, 0};
            //             break;
            //         case SDLK_SPACE:
            //             cameraVel += vec4{0, 1, 0, 0};
            //             break;
            //         case SDLK_LSHIFT:
            //             cameraVel += vec4{0, -1, 0, 0};
            //             break;
            //     }
            //     break;
            // case SDL_KEYUP:
            //     switch(event.key.keysym.sym) {
            //         case SDLK_w:
            //             cameraVel += vec4{0, 0, -1, 0};
            //             break;
            //         case SDLK_s:
            //             cameraVel += vec4{0, 0, 1, 0};
            //             break;
            //         case SDLK_a:
            //             cameraVel += vec4{1, 0, 0, 0};
            //             break;
            //         case SDLK_d:
            //             cameraVel += vec4{-1, 0, 0, 0};
            //             break;
            //         case SDLK_SPACE:
            //             cameraVel += vec4{0, -1, 0, 0};
            //             break;
            //         case SDLK_LSHIFT:
            //             cameraVel += vec4{0, 1, 0, 0};
            //             break;
            //     }
            //     break;
        }

        if (!running)
            break;

        if (!draw_surface)
		{
			draw_surface = SDL_CreateRGBSurfaceWithFormat(0, width, height, 32, SDL_PIXELFORMAT_RGBA32);
			SDL_SetSurfaceBlendMode(draw_surface, SDL_BLENDMODE_NONE);
		}

        auto now = clock::now();
		float dt = std::chrono::duration_cast<std::chrono::duration<float>>(now - prev_frame).count();
		float rt = std::chrono::duration_cast<std::chrono::duration<float>>(now - runtime).count();
        prev_frame = now;

        //E.G.: potential idea for frame lockstep changes
        //for(;dt < 0.16;dt = std::chrono::duration_cast<std::chrono::duration<float>>(n - prev_frame).count()){
        //    auto n = clock::now()
        //}
        

        //std::cout << dt << std::endl;
        
        
        stuff.models[0].transform = applyTranslation(stuff.models[0].transform, {0, static_cast<float>(std::sin(rt*(std::numbers::pi))) * 2 * dt, 0, 1});
        stuff.models[0].transform = applyRotationY(stuff.models[0].transform, (std::numbers::pi/2) * dt);
        
        if (length(cameraVel) > 0)
            stuff.cameraPos += normalisedVector(cameraVel) * dt;

        std::vector<uint32_t> colorBuffer = raster.rasterize(stuff, height, width);
        std::copy(colorBuffer.data(), colorBuffer.data() + (height*width), (uint32_t *)draw_surface->pixels);
        

        SDL_Rect rect{.x = 0, .y = 0, .w = width, .h = height};
        SDL_BlitSurface(draw_surface, &rect, SDL_GetWindowSurface(window), &rect);

        SDL_UpdateWindowSurface(window);
        FrameMark;
    }
}

