#include <stdlib.h>

#include <SDL2/SDL.h>
#include <complex>
#include <iostream>

static const int WINDOW_WIDTH = 800;
static const int WINDOW_HEIGHT = 800;

static const float LIMIT = 1.0f;

SDL_Event event;
SDL_Renderer* renderer;
SDL_Window* window;

float map(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

float Distance(const int x0, const int y0, const int x1, const int y1) {
	return sqrtf((x1 - x0) * (x1 - x0) + (y1 - y0) * (y1 - y0));
}

void DrawPointBlack(const int x, const int y) {
   

	SDL_RenderDrawPoint(renderer, WINDOW_WIDTH * 0.5 + x, WINDOW_HEIGHT * 0.5 + y);
}


void DrawPixel(const float x, const float y)
{
    float fx = map(x,-LIMIT,LIMIT,0,WINDOW_WIDTH);
    float fy = map(y,-LIMIT,LIMIT,0,WINDOW_HEIGHT);

    SDL_RenderDrawPoint(renderer, fx, fy);
}

void DrawPointRed(const int x, const int y) {
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
	SDL_RenderDrawPoint(renderer, WINDOW_WIDTH * 0.5 + x, WINDOW_HEIGHT * 0.5 + y);
}

void InitVideo() {
	SDL_Init(SDL_INIT_VIDEO);
	SDL_CreateWindowAndRenderer(WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL, &window, &renderer);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
	// SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
}

void DrawMandelbrot() {

    
     SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
for (float y = -LIMIT; y < LIMIT; y += 0.1f)
        {
    for (float x = -LIMIT; x < LIMIT; x += 0.1f)
    {
        


        for (size_t j = 0; j < 200; j++)
        {
            std::complex<float> n;
            n.real(0);
            n.imag(0);
            std::complex<float> c;
            c.real(x);
            c.imag(y);

            for (size_t i = 0; i < j; i++)
            {
               n = n*n + c ;
            //    if (sqrtf(n.real()*n.real() + n.imag()*n.imag()) > 2.0f)
            //      goto xloop;
            }
            
            
            DrawPixel(n.real(), n.imag());
            //DrawPointBlack(x,y);
            // DrawPointRed();

            //SDL_Delay(1);

            

            //std::cout << n.real() << ";" << n.imag() << std::endl;
            
        }

        }
        
           
        xloop:;
        
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