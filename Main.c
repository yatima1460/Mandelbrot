#include <SDL2/SDL.h>
#include <assert.h>
#include <complex.h>

// The window is a square for now
static const int WINDOW_PIXELS_SIDE_LENGTH = 1000;

// More iterations = slower but more sharp edges
// Less iterations = faster but blobby Mandelbrot
static const int ITERATIONS_PER_COMPLEX_NUMBER = 50;

// <1.0 = you risk to generate an image with holes (super fast)
// 1.0 = 1 complex point tested / pixel (normal nice image)
// >1.0 = more complex points tested / pixel (super-resolution SLOWER)
static const float PRECISION = 4.0F;

// The zoom level
// Mandelbrot doesn't exist above distance 2 with 0,0 so it's pointless so go above
static const float LIMIT = 2.0F;
static const float STEP = ((LIMIT * 2.0F) / WINDOW_PIXELS_SIDE_LENGTH) / PRECISION;

SDL_Event event;
SDL_Renderer* renderer;
SDL_Window* window;

float map(const float x, const float in_min, const float in_max, const float out_min, const float out_max) {
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

float* CalculateMandelbrot() {
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

	complex float n = {0};
	complex float c = {0};

	float* stability = (float*)malloc(sizeof(float) * WINDOW_PIXELS_SIDE_LENGTH * WINDOW_PIXELS_SIDE_LENGTH);
	if (stability == NULL) {
		fprintf(stderr, "Not enough memory!");
		abort();
	}
for (int px = 0; px < WINDOW_PIXELS_SIDE_LENGTH; px++) {
	for (int py = 0; py < WINDOW_PIXELS_SIDE_LENGTH; py++) {
		

			const float x = map(px, 0, WINDOW_PIXELS_SIDE_LENGTH,-LIMIT, LIMIT );
			const float y = map(py, 0, WINDOW_PIXELS_SIDE_LENGTH,-LIMIT, LIMIT );
			n = 0 + 0 * I;
			c = x + y * I;

			int i = 0;
			while (i < ITERATIONS_PER_COMPLEX_NUMBER) {
				// Multiply c = zoom level
				// Add complex = offset
				n = n * n + c;

				const float n_real = creal(n);
				const float n_imag = cimag(n);

				// If the complex number distance with the origin is > 2 than it's unstable
				if (n_real * n_real + n_imag * n_imag > 4.0) {
					
					// Stability color is based on number of iterations reached
					const float color = ((float)(i) / (float)(ITERATIONS_PER_COMPLEX_NUMBER));
					*(stability + py * WINDOW_PIXELS_SIDE_LENGTH + px) = color;
					break;
				}
				i++;
			}
		}
	}

	return stability;
}

void DrawMandelbrot(const float* const stabilityPoints) {
	assert(stabilityPoints != NULL);
	for (int i = 0; i < WINDOW_PIXELS_SIDE_LENGTH * WINDOW_PIXELS_SIDE_LENGTH; i++) {
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
	while (1) {
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
	const int beginTime = SDL_GetTicks();
	const float* stabilityPoints = CalculateMandelbrot();
	const int endTime = SDL_GetTicks();
	SDL_SetWindowTitle(window, "Mandelbrot - Status: Drawing...");
	DrawMandelbrot(stabilityPoints);
	free((void*)stabilityPoints);
	char s[200] = {0};
	sprintf(s, "Mandelbrot - Status: Done! Time: %dms", endTime - beginTime);
	SDL_SetWindowTitle(window, s);
	WaitForExit();
	DestroyVideo();

	return EXIT_SUCCESS;
}