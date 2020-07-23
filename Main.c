#include <SDL2/SDL.h>
#include <assert.h>
#include <complex.h>




static const int WINDOW_PIXELS_WIDTH = 1280;
static const int WINDOW_PIXELS_HEIGHT = 720;

// More iterations = slower but more sharp edges
// Less iterations = faster but blobby Mandelbrot
static const int ITERATIONS_PER_COMPLEX_NUMBER = 50;

// Color opacity * this
static const float COLOR_MULTIPLIER = 4.0f;

// The zoom level
// Mandelbrot doesn't exist above distance 2 with 0,0 so it's pointless to go above
static const float LIMIT = 2.0F;

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

SDL_Event event;
SDL_Renderer* renderer;
SDL_Window* window;

float map(const float x, const float in_min, const float in_max, const float out_min, const float out_max) {
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void InitVideo() {
	SDL_Init(SDL_INIT_VIDEO);
	SDL_CreateWindowAndRenderer(
		WINDOW_PIXELS_WIDTH, WINDOW_PIXELS_HEIGHT, 0, &window, &renderer);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
}

float* CalculateMandelbrot() {
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

	complex float n = {0};
	complex float c = {0};

	float* stability = (float*)malloc(sizeof(float) * WINDOW_PIXELS_WIDTH * WINDOW_PIXELS_HEIGHT);
	if (stability == NULL) {
		fprintf(stderr, "Not enough memory!");
		abort();
	}

	for (int i = 0; i < WINDOW_PIXELS_WIDTH * WINDOW_PIXELS_HEIGHT; i++) {
		const int px = i % WINDOW_PIXELS_WIDTH;
		const int py = i / WINDOW_PIXELS_WIDTH;

		const float offset = (WINDOW_PIXELS_WIDTH - WINDOW_PIXELS_HEIGHT)/2;
		const float x = map(px, offset, offset+WINDOW_PIXELS_HEIGHT, -LIMIT, LIMIT);
		const float y = map(py, 0, WINDOW_PIXELS_HEIGHT, -LIMIT, LIMIT);
		n = 0 + 0 * I;
		c = x + y * I;

		for (int j = 0; j < ITERATIONS_PER_COMPLEX_NUMBER; j++) {
			// Multiply c = zoom level
			// Add complex = offset
			n = n * n + c;

			const float n_real = creal(n);
			const float n_imag = cimag(n);

			// If the complex number distance with the origin is > 2 then it's unstable
			if (n_real * n_real + n_imag * n_imag > 4.0) {
				// Stability color is based on number of iterations reached
				*(stability + i) = ((float)(j) / (float)(ITERATIONS_PER_COMPLEX_NUMBER));
				break;
			}
		}
	}

	return stability;
}

void DrawMandelbrot(const float* const stabilityPoints) {
	assert(stabilityPoints != NULL);
	SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" );



	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	for (int i = 0; i < WINDOW_PIXELS_WIDTH * WINDOW_PIXELS_HEIGHT; i++) {
		const int x = i % WINDOW_PIXELS_WIDTH;
		const int y = i / WINDOW_PIXELS_WIDTH;
		const float color = stabilityPoints[i] * 255;

		SDL_SetRenderDrawColor(
			renderer, color, color, 255 - color, MIN(255 * stabilityPoints[i] * COLOR_MULTIPLIER, 255));

		SDL_RenderDrawPoint(renderer, x, y);
	}
	// Draw image on screen
	SDL_RenderPresent(renderer);
}

void WaitForExit() {
	while (1) {
		if (SDL_PollEvent(&event)) {
			
			if (event.type == SDL_QUIT || event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
				break;
			}
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