#include <iostream>
#include <SDL3/SDL.h>
using namespace std;
int main()
{
	cout << "Just a point\n";
	// 1) Init
	if (SDL_Init(SDL_INIT_VIDEO)) {
		cout << "Window initialization is successfull.\n";
	}
	else {
		cout << SDL_GetError();
		SDL_Quit();
		return -1;
	}
	// 2) create window
	SDL_Window* window = nullptr;
	window = SDL_CreateWindow("SDL3 Start", 1280, 720,
		SDL_WINDOW_VULKAN | SDL_WINDOW_HIGH_PIXEL_DENSITY);
	if (window == nullptr) {
		cout << "ERROR creating window\n";
		SDL_Quit();
		return -1;
	}
	else cout << "Window created successfully.\n";
	// 3 Write to surface directly
	// You may not combine this with 3D or the rendering API on this window.
	SDL_Surface* surface = SDL_GetWindowSurface(window);
	SDL_ClearSurface(surface, 0, 0, 0, 0);
	if (SDL_WriteSurfacePixel(surface, 10, 10, 255, 0, 0, 1)) {
		cout << "Pixel written successfully\n";
	}
	else {
		cout << "ERROR: Pixel\n";
	}
	SDL_UpdateWindowSurface(window);
	SDL_Delay(5000); // 5 Sekunden warten
	// 4) Clean up & close
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}