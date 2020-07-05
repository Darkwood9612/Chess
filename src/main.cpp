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
#include "DragDropManager.h"
#include "TextureStorage.h"
#include "Window.h"

#include "util.h"
#include "ChessDefines.h"

#undef main

namespace {
constexpr char* ENGINE_PATH = "engine\\stockfish_20011801_x64.exe";
constexpr char* WINDOW_TITLE = "Chess";
constexpr char g_BackgroundId = 'w';

constexpr uint8_t NUMBER_OF_FIELD_COLUMNS = 8;
constexpr uint8_t PIECE_IMAGE_SIZE = 90;

int Quit()
{
  ImGui_ImplOpenGL2_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();

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

  std::vector<std::pair<EPiece, std::string>> piecesImage = {
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
    { EPiece::B_BISHOP, getAbsoluatePath("image\\black_bishop_large.png") }
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

    for (auto& item : piecesImage) {
      textureStorage.ImageLoad((char)item.first, item.second);
    }
    textureStorage.ImageLoad(g_BackgroundId,
                             getAbsoluatePath("image\\green_board_large.png"));

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    bool done = false;

    ChessEngine engine(getAbsoluatePath(ENGINE_PATH));
    engine.StartNewGame();
    engine.UpdateFen();

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
        (void*)textureStorage.LookupTextureById(g_BackgroundId), ImVec2(0, 0),
        ImVec2(DISPLAY_SIZE_Y, DISPLAY_SIZE_Y));

      auto field = util::FennToArray(engine.GetFen());

      for (size_t x = 0; x < NUMBER_OF_FIELD_COLUMNS; ++x) {
        for (size_t y = 0; y < NUMBER_OF_FIELD_COLUMNS; ++y) {
          auto cellNumber = x + y * NUMBER_OF_FIELD_COLUMNS;
          auto piece = field[cellNumber];

          ImGui::SetNextWindowSize(ImVec2(PIECE_IMAGE_SIZE, PIECE_IMAGE_SIZE),
                                   ImGuiCond_Once);
          ImGui::SetNextWindowBgAlpha(0.0f);
          ImGui::SetNextWindowPos(
            ImVec2(x * CUBE_FACE_SIZE, y * CUBE_FACE_SIZE), ImGuiCond_Once);

          ImGuiWindowFlags pieceFlags = ImGuiWindowFlags_NoDecoration |
            ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollWithMouse;

          if (IsEmpty(piece) || (engine.NowWhiteMove() && IsBlack(piece)) ||
              (!engine.NowWhiteMove() && IsWhite(piece)))
            pieceFlags |= ImGuiWindowFlags_NoMove;

          ImGui::Begin(std::to_string(cellNumber).c_str(), NULL, pieceFlags);

          auto piecePos = ImGui::GetWindowPos();
          ImGui::GetStyle().WindowBorderSize = 0.f;

          if (ImGui::BeginDragDropSource()) {

            auto t = CUBE_FACE_SIZE / 2;
            std::string res =
              std::to_string(int((piecePos.x + t) / CUBE_FACE_SIZE) +
                             int((piecePos.y + t) / CUBE_FACE_SIZE) *
                               NUMBER_OF_FIELD_COLUMNS);

            ImGui::Text("Cell number %s", res.data());

            ImGui::EndDragDropSource();
          }

          auto g = ImGui::IsMouseDragging(0);
          ImGui::Text("Drag = %d", g);

          if (!IsEmpty(piece)) {

            ImGui::GetBackgroundDrawList()->AddImage(
              (void*)textureStorage.LookupTextureById(piece),
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

    return Quit();

  } catch (const std::exception& e) {
    MessageBox(NULL, e.what(), NULL, MB_OK);
  }
};