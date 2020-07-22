#include <stdlib.h>

#include <SDL2/SDL.h>

static const int WINDOW_WIDTH = 1280;
static const int WINDOW_HEIGHT = 720;



SDL_Event event;
SDL_Renderer *renderer;
SDL_Window *window;

void DrawPoint(int x, int y) 
{
    SDL_RenderDrawPoint(renderer, WINDOW_WIDTH*0.5+x, WINDOW_HEIGHT*0.5+y);
}

void InitVideo() {
  SDL_Init(SDL_INIT_VIDEO);
  SDL_CreateWindowAndRenderer(WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL, &window,
                              &renderer);
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
  SDL_RenderClear(renderer);
  SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
}

void DrawMandelbrot() {
  for (int i = 0; i < WINDOW_WIDTH; ++i)
    DrawPoint(i,i);
    

  SDL_RenderPresent(renderer);
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