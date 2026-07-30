#include <iostream>
#include "SDL.h"

int const WIDTH = 1920;
int const HEIGHT = 1080;


int main(int argc, char* args[]){
    SDL_Window* window = SDL_CreateWindow("Terraria",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        WIDTH, HEIGHT,
        SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN);
}

