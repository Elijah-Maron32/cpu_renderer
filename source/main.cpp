#include <iostream>
#include "SDL.h"
#include <chrono>
#include <renderer/modelImporter.hpp>

#define FPS 60.0
#define TARGET_DT 1.0 / FPS

using namespace renderer;

int main(int argc, char* args[]){

    renderer::vec4 const cameraPos = {0,1,5,1};

    renderer::model m;

    renderer::parseOBJFile("cube.obj", m.vertexPositions, m.faceVerticies);

    std::cout << "output\n";

    for (renderer::vec4 element : m.vertexPositions)
    std::cout << element['x'] << "," << element['g']  << "," << element['B'] << "," << element['W'] << " ";

    std::cout << "\n";

    for (renderer::Face element : m.faceVerticies)
    std::cout << element[0] << "," << element[1]  << "," << element[2] << " ";

    std::cout << "\n";

    int width = 1920;
    int height = 1080;
    
    SDL_Window* window = SDL_CreateWindow("Terraria",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        width, height,
        SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN);

    SDL_Surface * draw_surface = nullptr;

    int mouse_x = 0;
    int mouse_y = 0;

    using clock = std::chrono::high_resolution_clock;
    auto prev_frame = clock::now();

    bool running = true;
    while (running)
    {
        for (SDL_Event event; SDL_PollEvent(&event);) switch (event.type)
        {
        case SDL_WINDOWEVENT:
            switch (event.window.event)
            {
            case SDL_WINDOWEVENT_RESIZED:
                if (draw_surface)
					SDL_FreeSurface(draw_surface);
				draw_surface = nullptr;
                width = event.window.data1;
                height = event.window.data2;
                break;
            }
            break;
        case SDL_QUIT:
            running = false;
            break;
        case SDL_MOUSEMOTION:
            mouse_x = event.motion.x;
            mouse_y = event.motion.y;
            break;
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
		prev_frame = now;

        //std::cout << dt << std::endl;

        std::fill_n((uint32_t *)draw_surface->pixels, width * height, 0xff0000ff);

        SDL_Rect rect{.x = 0, .y = 0, .w = width, .h = height};
        SDL_BlitSurface(draw_surface, &rect, SDL_GetWindowSurface(window), &rect);

        SDL_UpdateWindowSurface(window);
    }
}

