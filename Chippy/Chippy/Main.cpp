#include <iostream>
#include <SDL.h>
#include "chip8.h"
using namespace std;

int main(int argc, char *argv[])
{
	chip8 emulator;
	emulator.run();

	//The window we'll be rendering to
	SDL_Window* Window = NULL;

	//The surface contained by the window
	SDL_Surface* ScreenSurface = NULL;

	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
	}
	else
	{
		//Create window
		Window = SDL_CreateWindow("There's my chippy", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 320, SDL_WINDOW_SHOWN);
		if (Window == NULL)
		{
			printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
		}
		else
		{
			//Get window surface
			ScreenSurface = SDL_GetWindowSurface(Window);
		}
	}

	SDL_Renderer *renderer = SDL_CreateRenderer(Window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (renderer == nullptr)
	{
		SDL_DestroyWindow(Window);
		std::cout << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return 1;
	}
	bool quit = false;
	SDL_Event e;
	while (!quit)
	{
		while (SDL_PollEvent(&e))
		{
			if (e.type == SDL_QUIT)
			{
				quit = true;
			}
		}
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Clear to black
		SDL_RenderClear(renderer);
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Draw white
		SDL_Rect rect;
		rect.x = 50;
		rect.y = 50;
		rect.h = 50;
		rect.w = 50;
		SDL_RenderFillRect(renderer, &rect);
		SDL_RenderPresent(renderer);
		emulator.update(0);
	}
	SDL_Quit();
	return 0;
}