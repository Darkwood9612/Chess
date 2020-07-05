#include <SDL.h>
#include <SDL_image.h>
#include <SDL_opengl.h>
#include <SDL_ttf.h>

#include "imgui.h"
#include "imgui_impl_opengl2.h"
#include "imgui_impl_sdl.h"

#include "imgui_impl_opengl2.cpp"
#include "imgui_impl_sdl.cpp"

#include <filesystem>
#include <iostream>
#include <stdexcept>

#include "ChessEngine.h"
#include "GameModel.h"
#include "TextureStorage.h"
#include "Window.h"

#undef main

namespace {

constexpr char* FONT_PATH = "font\\fast99.ttf";
constexpr int FONT_SIZE = 36;

constexpr char* ENGINE_PATH = "engine\\stockfish_20011801_x64.exe";
constexpr char* WINDOW_TITLE = "Chess";

constexpr uint8_t NUMBER_OF_FIELD_COLUMNS = 8;
constexpr uint8_t PIECE_IMAGE_SIZE = 90;

int Quit(Window* window)
{

  window ? SDL_DestroyWindow(window->window)
         : throw std::runtime_error("window == nullptr");
  ImGui_ImplOpenGL2_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();

  SDL_GL_DeleteContext(window->gl_context);
  IMG_Quit();
  TTF_Quit();
  SDL_Quit();

  return 0;
}

void Init()
{
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) !=
      0) {
    std::string err = SDL_GetError();
    throw std::runtime_error("Fatal initialization SDL error: " + err);
  }

  if (TTF_Init() != 0) {
    std::string err = TTF_GetError();
    throw std::runtime_error("Fatal initialization TTF error: " + err);
  }

  if (IMG_Init(IMG_INIT_PNG) == 0) {
    std::string err = IMG_GetError();
    throw std::runtime_error("Fatal initialization IMG error: " + err);
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
    Init();

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();

    Window window = Window(
      WINDOW_TITLE, SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI, 1235, 742);
    if (!window.window)
      throw std::runtime_error("window == nullptr");

    ImGui::StyleColorsDark();
    ImGui_ImplSDL2_InitForOpenGL(window.window, window.gl_context);
    ImGui_ImplOpenGL2_Init();

    TextureStorage textureStorage;

    textureStorage.LoadFieldBackround(
      getAbsoluatePath("image\\green_board_large.png"));

    textureStorage.LoadPiecesImage({
      { EPiece::W_PAWN, getAbsoluatePath("image\\white_pawn_large.png") },
      { EPiece::W_ROOK, getAbsoluatePath("image\\white_rook_large.png") },
      { EPiece::W_QUEEN, getAbsoluatePath("image\\white_queen_large.png") },
      { EPiece::W_KNIGHT, getAbsoluatePath("image\\white_knight_large.png") },
      { EPiece::W_KING, getAbsoluatePath("image\\white_king_large.png") },
      { EPiece::W_BISHOP, getAbsoluatePath("image\\white_bishop_large.png") },

      { EPiece::B_PAWN, getAbsoluatePath("image\\black_pawn_large.png") },
      { EPiece::B_ROOK, getAbsoluatePath("image\\black_rook_large.png") },
      { EPiece::B_QUEEN, getAbsoluatePath("image\\black_queen_large.png") },
      { EPiece::B_KNIGHT, getAbsoluatePath("image\\black_knight_large.png") },
      { EPiece::B_KING, getAbsoluatePath("image\\black_king_large.png") },
      { EPiece::B_BISHOP, getAbsoluatePath("image\\black_bishop_large.png") },
    });

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    bool done = false;

    ChessEngine engine(getAbsoluatePath(ENGINE_PATH));
    Game game(&engine);

    game.StartNewGame();
    game.UpdateField();

    engine.PositionStartPos("a2a4");
    game.UpdateField();

    engine.Position(game.GetFEN(), "a7a5");
    game.UpdateField();

    while (!done) {

      SDL_Event event;
      while (SDL_PollEvent(&event)) {
        ImGui_ImplSDL2_ProcessEvent(&event);
        if (event.type == SDL_QUIT)
          done = true;
      }

      ImGui_ImplOpenGL2_NewFrame();
      ImGui_ImplSDL2_NewFrame(window.window);

      const uint32_t DISPLAY_SIZE_Y = io.DisplaySize.y;
      const uint32_t DISPLAY_SIZE_X = io.DisplaySize.x;
      const float CUBE_FACE_SIZE = DISPLAY_SIZE_Y / NUMBER_OF_FIELD_COLUMNS;

      ImGui::NewFrame();

      ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Once);
      ImGui::SetNextWindowSize(ImVec2(DISPLAY_SIZE_Y, DISPLAY_SIZE_Y),
                               ImGuiCond_Once);
      ImGui::SetNextWindowBgAlpha(0.0f);

      ImGui::Begin("Game", NULL,
                   ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
                     ImGuiWindowFlags_NoScrollWithMouse);

      ImGui::GetBackgroundDrawList()->AddImage(
        (void*)textureStorage.GetFieldBackroundTextureId(), ImVec2(0, 0),
        ImVec2(DISPLAY_SIZE_Y, DISPLAY_SIZE_Y));

      auto field = game.GetFieldData();

      for (size_t x = 0; x < NUMBER_OF_FIELD_COLUMNS; ++x) {
        for (size_t y = 0; y < NUMBER_OF_FIELD_COLUMNS; ++y) {

          auto pice = field[(x + y * NUMBER_OF_FIELD_COLUMNS)];

          ImGui::SetNextWindowSize(ImVec2(PIECE_IMAGE_SIZE, PIECE_IMAGE_SIZE),
                                   ImGuiCond_Once);
          ImGui::SetNextWindowBgAlpha(0.0f);
          ImGui::SetNextWindowPos(
            ImVec2(x * CUBE_FACE_SIZE, y * CUBE_FACE_SIZE), ImGuiCond_Once);

          ImGuiWindowFlags pieceFlags = ImGuiWindowFlags_NoDecoration |
            ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollWithMouse;

          if (!pice.isMovable)
            pieceFlags |= ImGuiWindowFlags_NoMove;

          ImGui::Begin(std::to_string(x + y * NUMBER_OF_FIELD_COLUMNS).c_str(),
                       NULL, pieceFlags);

          auto piecePos = ImGui::GetWindowPos();
          ImGui::GetStyle().WindowBorderSize = 0.f;

          if (ImGui::BeginDragDropSource()) {

              auto t = CUBE_FACE_SIZE / 2;
            std::string res = std::to_string(int((piecePos.x + t) / CUBE_FACE_SIZE) +
                                             int((piecePos.y+t) / CUBE_FACE_SIZE) *
                                               NUMBER_OF_FIELD_COLUMNS);

            ImGui::Text("Cell number \"%s\"", res.data());

            ImGui::EndDragDropSource();
          }

          auto g = ImGui::IsMouseDragging();

          if (pice.type != EPiece::EMPTY) {

            ImGui::GetBackgroundDrawList()->AddImage(
              (void*)textureStorage.GetPieceTextureId(pice),
              ImVec2(piecePos.x, piecePos.y),
              ImVec2(PIECE_IMAGE_SIZE + piecePos.x,
                     PIECE_IMAGE_SIZE + piecePos.y));
          }

          ImGui::End();
        }
      }

      ImGui::End();

      ImGui::SetNextWindowPos(ImVec2(DISPLAY_SIZE_Y, 0), ImGuiCond_Once);
      ImGui::SetNextWindowSize(
        ImVec2(DISPLAY_SIZE_X - DISPLAY_SIZE_Y, DISPLAY_SIZE_Y),
        ImGuiCond_Once);

      ImGui::Begin("Options", &done,
                   ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

      ImGui::Text("ms/frame (%.1f FPS)", io.Framerate);

      for (auto& str : engine.GetAnswers())
        ImGui::Text("%s", str.data());

      ImGui::End();

      ImGui::Render();
      glViewport(0, 0, DISPLAY_SIZE_X, DISPLAY_SIZE_Y);
      glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
      glClear(GL_COLOR_BUFFER_BIT);

      ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
      SDL_GL_SwapWindow(window.window);
    }

    return Quit(&window);

  } catch (const std::exception& e) {
    MessageBox(NULL, e.what(), NULL, MB_OK);
  }
};