#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <Windows.h>

#include "GameModel.h"
#include "View.h"
#include "Window.h"

#include <filesystem>
#include <stdexcept>

#include "ChessEngine.h"
#include <iostream>

#undef main

namespace {

constexpr char* FONT_PATH = "font\\fast99.ttf";
constexpr int FONT_SIZE = 36;

constexpr char* WINDOW_TITLE = "Chess";

int Quit(SDL_Window* window)
{

  window ? SDL_DestroyWindow(window)
         : throw std::runtime_error("window == nullptr");

  TTF_Quit();
  SDL_Quit();

  return 0;
}

void Init()
{
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    std::string err = SDL_GetError();
    throw std::runtime_error("Fatal initialization SDL error: " + err);
  }

  if (TTF_Init() != 0) {
    std::string err = TTF_GetError();
    throw std::runtime_error("Fatal initialization TTF error: " + err);
  }
}
}

int main(int argc, char** args)
{

  // while (!IsDebuggerPresent()) {
  //    Sleep(1);
  //}

  setlocale(0, "");
  std::string tmp;
  auto getAbsoluatePath = [&](const char* relativePath) {
    tmp =
      (std::filesystem::path(args[0]).parent_path() / relativePath).string();
    return tmp.c_str();
  };

  try {

    Window window = Window(WINDOW_TITLE, SDL_WINDOW_SHOWN, 1024, 768);
    if (!window.window)
      throw std::runtime_error("window == nullptr");

    ChessEngine engine("./stockfish_20011801_x64.exe");
    Sleep(1000);
    std::cout << engine.GetAnswer() << std::endl;

    engine.SendCommand("isready");
    Sleep(1000);

    engine.Position("N7/P3pk1p/3p2p1/r4p2/8/4b2B/4P1KP/1R6 w - - 0 34");
    Sleep(1000);

    engine.SendCommand("d");
    std::cout << engine.GetAnswer() << std::endl;
    Sleep(5000);

    engine.SetOption("MultiPV", "3");
    engine.SendCommand("go movetime 30000");
    std::cout << engine.GetAnswer() << std::endl;
    Sleep(31000);
    std::cout << engine.GetAnswer() << std::endl;

    engine.Position("N7/P3pk1p/3p2p1/r4p2/8/4b2B/4P1KP/R7 b - - 1 34");
    Sleep(1000);

    engine.SendCommand("d");
    std::cout << engine.GetAnswer() << std::endl;
    Sleep(5000);

    engine.SendCommand("go movetime 30000");
    std::cout << engine.GetAnswer() << std::endl;
    Sleep(31000);
    std::cout << engine.GetAnswer() << std::endl;

    Sleep(5000);

    return Quit(window.window);

  } catch (const std::exception& e) {
    MessageBox(NULL, e.what(), NULL, MB_OK);
  }
};