#pragma once
#include <SDL2\SDL_video.h>

class Window
{
public:
  Window(const char* title, uint32_t windowFlags = SDL_WINDOW_SHOWN,
         const int _SCREEN_WIDTH = 640, const int _SCREEN_HEIGHT = 480);

  SDL_Surface* screen = nullptr;
  SDL_Window* window = nullptr;
  const int SCREEN_WIDTH = 640;
  const int SCREEN_HEIGHT = 480;

private:
};