#include <stdlib.h>

#include <SDL2/SDL.h>
#include <complex>
#include <iostream>

static const int WINDOW_SIDE = 1000;
static const int ITERATIONS = 200;


// <1.0 = you risk to generate an image with holes (super fast)
// 1.0 = 1 complex point tested / pixel (normal nice image)
// >1.0 = more complex points tested / pixel (super-resolution SLOWER)
static const float PRECISION = 4.0f;

// static const float LIMIT = 2.0;
// static const float STEP = 0.004;
// static const int ITERATIONS = 500;

// The zoom level
static const float LIMIT = 2.0;

static const float STEP = ((LIMIT*2.0)/WINDOW_SIDE)/PRECISION;

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
	SDL_RenderDrawPoint(renderer, WINDOW_SIDE * 0.5 + x, WINDOW_SIDE * 0.5 + y);
}

void DrawPixel(const float x, const float y) {
	float fx = map(x, -LIMIT, LIMIT, 0, WINDOW_SIDE);
	float fy = map(y, -LIMIT, LIMIT, 0, WINDOW_SIDE);

	SDL_RenderDrawPoint(renderer, fx, fy);
}

void DrawPointRed(const int x, const int y) {
	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
	SDL_RenderDrawPoint(renderer, WINDOW_SIDE * 0.5 + x, WINDOW_SIDE * 0.5 + y);
}

void InitVideo() {
	SDL_Init(SDL_INIT_VIDEO);
	SDL_CreateWindowAndRenderer(WINDOW_SIDE, WINDOW_SIDE, SDL_WINDOW_OPENGL, &window, &renderer);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
	// SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
}

void DrawMandelbrot() {
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);


	for (float y = -LIMIT; y < LIMIT; y += STEP) {
		for (float x = -LIMIT; x < LIMIT; x += STEP) {
			// for (size_t j = 0; j < ITERATIONS; j++) {
			std::complex<float> n;
			n.real(0);
			n.imag(0);
			std::complex<float> c;
			c.real(x);
			c.imag(y);

			
			for (size_t i = 0; i < ITERATIONS; i++) {
				n = n * n + c;
                if (sqrtf(n.real() * n.real() + n.imag() * n.imag()) > 2.0) 
                {
				    float fx = map(x, -LIMIT, LIMIT, 0, WINDOW_SIDE);
                    float fy = map(y, -LIMIT, LIMIT, 0, WINDOW_SIDE);
                    float color = ((float)i / (float)ITERATIONS)*255;
                    SDL_SetRenderDrawColor(renderer, color, color, color, 255);
                    SDL_RenderDrawPoint(renderer, fx, fy);
                    break;
			    }
			}

			
            
		}

    

		SDL_RenderPresent(renderer);
	}

	std::cout << "Done." << std::endl;

	SDL_RenderPresent(renderer);

	// for (int i = 0; i < WINDOW_WIDTH; ++i)
	// 	DrawPoint(i, i);
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