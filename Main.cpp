

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
static const float PRECISION = 4.0F;

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

constexpr float map(const float x,const  float in_min,const  float in_max,const  float out_min,const  float out_max) {
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


void InitVideo() {
	SDL_Init(SDL_INIT_VIDEO);
	SDL_CreateWindowAndRenderer(
		WINDOW_PIXELS_SIDE_LENGTH, WINDOW_PIXELS_SIDE_LENGTH, SDL_WINDOW_OPENGL, &window, &renderer);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
	// SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
}

std::vector<float> CalculateMandelbrot() {
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    std::complex<float> n;
    std::complex<float> c;

    
	std::vector<float> stability(WINDOW_PIXELS_SIDE_LENGTH * WINDOW_PIXELS_SIDE_LENGTH);
	for (float x = -LIMIT; x < LIMIT; x += STEP) {
		for (float y = -LIMIT; y < LIMIT; y += STEP) {

			n.real(0);
            n.imag(0);
            c.real(x);
            c.imag(y);

			for (int i = 0; i < ITERATIONS_PER_COMPLEX_NUMBER; i++) {
				// Multiply c = zoom level
				// Add complex = offset
				n = n * n + c;

                // If the complex number distance with the origin is > 2 than it's unstable
				if (n.real() * n.real() + n.imag() * n.imag() > 4.0) {
					const int fx = static_cast<int>(map(x, -LIMIT, LIMIT, 0, WINDOW_PIXELS_SIDE_LENGTH));
					const int fy = static_cast<int>(map(y, -LIMIT, LIMIT, 0, WINDOW_PIXELS_SIDE_LENGTH));
                    // Stability color is based on number of iterations reached
					const float color = (static_cast<float>(i) / static_cast<float>(ITERATIONS_PER_COMPLEX_NUMBER));
					stability[fy * WINDOW_PIXELS_SIDE_LENGTH + fx] = color;
					break;
				}
			}
		}
	}

    return stability;

	
}

void DrawMandelbrot(const std::vector<float> stabilityPoints)
{
    for (size_t i = 0; i < WINDOW_PIXELS_SIDE_LENGTH * WINDOW_PIXELS_SIDE_LENGTH; i++) {
		const int x = i % WINDOW_PIXELS_SIDE_LENGTH;
		const int y = i / WINDOW_PIXELS_SIDE_LENGTH;
		const float color = stabilityPoints[i] * 255;
		SDL_SetRenderDrawColor(renderer, color, color, color, 255);
		SDL_RenderDrawPoint(renderer, x, y);
	}
    // Draw image on screen
	SDL_RenderPresent(renderer);
}

void WaitForExit() {
	while (true) {
		if (SDL_PollEvent(&event) && event.type == SDL_QUIT) {
			break;
		}
	}
}

void DestroyVideo() {
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

int main() {
    SDL_SetWindowTitle(window, "Mandelbrot - Status: Initializing...");
	InitVideo();
    SDL_SetWindowTitle(window, "Mandelbrot - Status: Generating...");
    const auto beginTime = SDL_GetTicks();
    const auto stabilityPoints = CalculateMandelbrot();
    const auto endTime = SDL_GetTicks();
    SDL_SetWindowTitle(window, "Mandelbrot - Status: Drawing...");
	DrawMandelbrot(stabilityPoints);
    std::stringstream ss;
    ss << "Mandelbrot - Status: Done! Time: ";
    ss << endTime - beginTime;
    ss << "ms";
    SDL_SetWindowTitle(window, ss.str().c_str());
	WaitForExit();
	DestroyVideo();

	return EXIT_SUCCESS;
}