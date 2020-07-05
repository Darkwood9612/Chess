#pragma once
#include <SDL2\SDL_video.h>

class Window
{
public:
  Window(const char* title, uint32_t windowFlags,
         const int _SCREEN_WIDTH, const int _SCREEN_HEIGHT);
  ~Window();

  SDL_Window* window = nullptr;
  SDL_GLContext gl_context;
  const int SCREEN_WIDTH = 640;
  const int SCREEN_HEIGHT = 480;
};