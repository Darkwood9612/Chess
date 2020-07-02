#include <SDL2\SDL_ttf.h>

#include "View.h"

void View::Draw(GameModel& gameModel, Window& window)
{

  SDL_FillRect(window.screen, NULL,
               SDL_MapRGB(window.screen->format, 0, 0, 0));

  SDL_UpdateWindowSurface(window.window);
}