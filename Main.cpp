#include <stdlib.h>

#include <SDL2/SDL.h>
#include <complex>
#include <iostream>
#include <vector>
// The window is a square for now
static const int WINDOW_PIXELS_SIDE_LENGTH = 1000;

// More iterations = slower but more sharp edges
// Less iterations = faster but blobby Mandelbrot
static const int ITERATIONS_PER_COMPLEX_NUMBER = 50;

// <1.0 = you risk to generate an image with holes (super fast)
// 1.0 = 1 complex point tested / pixel (normal nice image)
// >1.0 = more complex points tested / pixel (super-resolution SLOWER)
static const float PRECISION = 4.0f;

// static const float LIMIT = 2.0;
// static const float STEP = 0.004;
// static const int ITERATIONS = 500;

// The zoom level
// Mandelbrot doesn't exist above distance 2 with 0,0 so it's pointless so go above
static const float LIMIT = 2.0;
static const float STEP = ((LIMIT * 2.0) / WINDOW_PIXELS_SIDE_LENGTH) / PRECISION;

SDL_Event event;
SDL_Renderer* renderer;
SDL_Window* window;

float map(float x, float in_min, float in_max, float out_min, float out_max) {
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

float Distance(const int x0, const int y0, const int x1, const int y1) {
	return sqrt((x1 - x0) * (x1 - x0) + (y1 - y0) * (y1 - y0));
}

void DrawPointBlack(const int x, const int y) {
	SDL_RenderDrawPoint(renderer, WINDOW_PIXELS_SIDE_LENGTH * 0.5 + x, WINDOW_PIXELS_SIDE_LENGTH * 0.5 + y);
}

void DrawPixel(const float x, const float y) {
	float fx = map(x, -LIMIT, LIMIT, 0, WINDOW_PIXELS_SIDE_LENGTH);
	float fy = map(y, -LIMIT, LIMIT, 0, WINDOW_PIXELS_SIDE_LENGTH);

	SDL_RenderDrawPoint(renderer, fx, fy);
}

void DrawPointRed(const int x, const int y) {
	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
	SDL_RenderDrawPoint(renderer, WINDOW_PIXELS_SIDE_LENGTH * 0.5 + x, WINDOW_PIXELS_SIDE_LENGTH * 0.5 + y);
}

void InitVideo() {
	SDL_Init(SDL_INIT_VIDEO);
	SDL_CreateWindowAndRenderer(
		WINDOW_PIXELS_SIDE_LENGTH, WINDOW_PIXELS_SIDE_LENGTH, SDL_WINDOW_OPENGL, &window, &renderer);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
	// SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
}

void DrawMandelbrot() {
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

	std::vector<float> stability;
	for (float x = -LIMIT; x < LIMIT; x += STEP) {
		for (float y = -LIMIT; y < LIMIT; y += STEP) {
			std::complex<float> n;
			std::complex<float> c(x, y);

			for (int i = 0; i < ITERATIONS_PER_COMPLEX_NUMBER; i++) {
				n = n * n + c;
				if (n.real() * n.real() + n.imag() * n.imag() > 4.0) {
					int fx = (int)map(x, -LIMIT, LIMIT, 0, WINDOW_PIXELS_SIDE_LENGTH);
					int fy = (int)map(y, -LIMIT, LIMIT, 0, WINDOW_PIXELS_SIDE_LENGTH);
					float color = ((float)i / (float)ITERATIONS_PER_COMPLEX_NUMBER) * 255;
                    stability.push_back(color);
                    
					//stability[fy * WINDOW_PIXELS_SIDE_LENGTH + fx] = color;
					break;
				}
			}
		}
	}
	for (size_t i = 0; i < WINDOW_PIXELS_SIDE_LENGTH * WINDOW_PIXELS_SIDE_LENGTH; i++) {
		const int x = i % WINDOW_PIXELS_SIDE_LENGTH;
		const int y = i / WINDOW_PIXELS_SIDE_LENGTH;
		const float color = stability[i];
		SDL_SetRenderDrawColor(renderer, color, color, color, 255);
		SDL_RenderDrawPoint(renderer, x, y);
	}
	SDL_RenderPresent(renderer);
	SDL_SetWindowTitle(window, "Mandelbrot - Status: Done!");
	
}

void WaitForExit() {
	while (1) {
		if (SDL_PollEvent(&event) && event.type == SDL_QUIT)
			break;
	}
}

void DestroyVideo() {
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

int main(void) {
	InitVideo();
	DrawMandelbrot();
	WaitForExit();
	DestroyVideo();

	return EXIT_SUCCESS;
}