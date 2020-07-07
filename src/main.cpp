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
#include <queue>
#include <stdexcept>

#include "ChessDefines.h"
#include "ChessEngine.h"
#include "CommandGenerator.h"
#include "DragDropManager.h"
#include "Move.h"
#include "TextureStorage.h"
#include "Window.h"
#include "util.h"

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
void AddImageToPieceBackgroundDrawList(char piece, ImVec2 piecePos,
                                       TextureStorage& textureStorage)
{
  if (IsEmpty(piece))
    return;

  ImGui::GetBackgroundDrawList()->AddImage(
    (void*)textureStorage.LookupTextureById(piece),
    ImVec2(piecePos.x, piecePos.y),
    ImVec2(PIECE_IMAGE_SIZE + piecePos.x, PIECE_IMAGE_SIZE + piecePos.y));
}
}

int main(int argc, char** args)
{
  setlocale(0, "");

  std::string tmpPath;
  auto getAbsoluatePath = [&tmpPath, args](const char* relativePath) {
    tmpPath =
      (std::filesystem::path(args[0]).parent_path() / relativePath).string();
    return tmpPath.c_str();
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
      WINDOW_TITLE, SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI, 742, 742);
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

    ChessEngine engine(getAbsoluatePath(ENGINE_PATH));
    ChessEngine engineReval(getAbsoluatePath(ENGINE_PATH));

    DragDropManager dragDropManager;
    TurnControl turnControl;
    Move move;

    std::queue<std::string> uiNotifications;

    auto startGameAndChoseRival = [&engine, &engineReval,
                                   &turnControl](bool playerWontPlayWithAI) {
      engine.SendCommand(CommandGenerator::NewGame());
      Sleep(300);
      engine.UpdateFen();
      engineReval.SendCommand(CommandGenerator::NewGame());
      Sleep(300);
      engineReval.UpdateFen();

      turnControl.SetPlayerMadeChoice(playerWontPlayWithAI);
    };

    dragDropManager.SetDragCallback([&move](int8_t dragCellId) {
      move.SetFrom(dragCellId, NUMBER_OF_FIELD_COLUMNS);
    });

    dragDropManager.SetDropCallback(
      [&engine, &engineReval, &turnControl, &move,
       &uiNotifications](bool isAI, int8_t dropCellId) {
        if (!isAI)
          move.SetTo(dropCellId, NUMBER_OF_FIELD_COLUMNS);

        auto fen = engine.GetFen();
        engine.SendCommand(CommandGenerator::Position(fen, move.GetMove()));
        engine.UpdateFen();

        bool isMoveWrong = engine.GetFen() == fen && !move.IsFromEqualTo();

        if (isMoveWrong) {
          std::string info =
            "\tError: move " + move.GetMove() + " is considered unacceptable!";
          uiNotifications.push(info);
        }

        switch (engine.IsSomebodyWon()) {
          case ChessEngine::WinningSide::Black:
            uiNotifications.push("\tBlack Player Win. GameOver!");
            turnControl.StartNewGame();
            return;
          case ChessEngine::WinningSide::White:
            uiNotifications.push("\tWhite Player Win. GameOver!");
            turnControl.StartNewGame();
            return;
          case ChessEngine::WinningSide::NoOne:

            if ((!isMoveWrong && !engine.NowWhiteMove()) ||
                (engine.GetFen() == fen && !engine.NowWhiteMove())) {

              engineReval.SendCommand(
                CommandGenerator::Position(engine.GetFen()));

              engineReval.SendCommand(CommandGenerator::GoSearchInfinite());
              turnControl.SetStartThinkTime();
            }
        }
      });

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    bool needToGoOut = false;

    while (!needToGoOut) {

      SDL_Event event;
      while (SDL_PollEvent(&event)) {
        ImGui_ImplSDL2_ProcessEvent(&event);
        if (event.type == SDL_QUIT)
          needToGoOut = true;
      }

      /// Say AI make a move if he thought enough
      if (turnControl.IsPlayerMadeChoice() &&
          turnControl.IsPlayerPlayWithAI() && !engine.NowWhiteMove() &&
          turnControl.IsEngineThoughtEnough()) {

        engineReval.SendCommand(CommandGenerator::Stop());
        engineReval.IsReady();

        std::vector<std::string> answers;
        util::split(engineReval.GetLastAnswer(), answers, " ");

        for (size_t i = 0; i < answers.size(); ++i) {
          if (answers[i].find("bestmove") != std::string::npos) {
            move.SetMove(answers[i + 1]);
            dragDropManager.DropItemAI();
          }
        }
      }

      ImGui_ImplOpenGL2_NewFrame();
      ImGui_ImplSDL2_NewFrame(window.window);

      const float CUBE_FACE_SIZE = io.DisplaySize.y / NUMBER_OF_FIELD_COLUMNS;

      ImGui::NewFrame();

      /// Draw Board
      {
        ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Once);
        ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x, io.DisplaySize.y),
                                 ImGuiCond_Once);
        ImGui::SetNextWindowBgAlpha(0.0f);

        ImGui::Begin("Game", NULL,
                     ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
                       ImGuiWindowFlags_NoScrollWithMouse);

        ImGui::GetBackgroundDrawList()->AddImage(
          (void*)textureStorage.LookupTextureById(g_BackgroundId),
          ImVec2(0, 0), ImVec2(io.DisplaySize.x, io.DisplaySize.y));

        auto field = util::FennToArray(engine.GetFen());

        for (size_t x = 0; x < NUMBER_OF_FIELD_COLUMNS; ++x) {
          for (size_t y = 0; y < NUMBER_OF_FIELD_COLUMNS; ++y) {

            auto cellNumber = x + y * NUMBER_OF_FIELD_COLUMNS;
            auto piece = field[cellNumber];

            if (!ImGui::IsMouseDragging(0) && !dragDropManager.IsEmpty()) {
              dragDropManager.DropItem();
            }

            /// if the piece is dragged, it will be drawn last
            if (!dragDropManager.IsEmpty() &&
                dragDropManager.GetDragItemName() ==
                  std::to_string(cellNumber)) {
              continue;
            }

            /// Draw Board Piece
            ImGui::SetNextWindowSize(
              ImVec2(PIECE_IMAGE_SIZE, PIECE_IMAGE_SIZE), ImGuiCond_Once);
            ImGui::SetNextWindowBgAlpha(0.0f);
            ImGui::SetNextWindowPos(
              ImVec2(x * CUBE_FACE_SIZE + 3, y * CUBE_FACE_SIZE));

            ImGuiWindowFlags pieceFlags = ImGuiWindowFlags_NoDecoration |
              ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollWithMouse;

            bool pieceCannotBeMoved = IsEmpty(piece) ||
              (engine.NowWhiteMove() && IsBlack(piece)) ||
              (!engine.NowWhiteMove() && IsWhite(piece)) ||
              !turnControl.IsPlayerMadeChoice() ||
              (turnControl.IsPlayerPlayWithAI() && IsBlack(piece));

            if (pieceCannotBeMoved)
              pieceFlags |= ImGuiWindowFlags_NoMove;

            ImGui::Begin(std::to_string(cellNumber).c_str(), NULL, pieceFlags);

            auto piecePos = ImGui::GetWindowPos();
            ImGui::GetStyle().WindowBorderSize = 0.f;

            if (ImGui::BeginDragDropSource()) {
              auto halfSquare = CUBE_FACE_SIZE / 2;

              if (!pieceCannotBeMoved)
                dragDropManager.SetDragItem(
                  std::to_string(cellNumber),
                  (piecePos.x + halfSquare) / CUBE_FACE_SIZE,
                  (piecePos.y + halfSquare) / CUBE_FACE_SIZE,
                  NUMBER_OF_FIELD_COLUMNS, piece);

              ImGui::EndDragDropSource();
            }

            if (turnControl.IsPlayerMadeChoice())
              AddImageToPieceBackgroundDrawList(piece, piecePos,
                                                textureStorage);

            ImGui::End();
          }
        }
      }

      /// Draw Dragging Cell
      {
        if (!dragDropManager.IsEmpty()) {
          ImGui::SetNextWindowSize(ImVec2(PIECE_IMAGE_SIZE, PIECE_IMAGE_SIZE),
                                   ImGuiCond_Once);
          ImGui::SetNextWindowBgAlpha(0.0f);
          ImGui::SetNextWindowPos(
            ImVec2(dragDropManager.GetDragItemStartPosX() * CUBE_FACE_SIZE,
                   dragDropManager.GetDragItemStartPosY() * CUBE_FACE_SIZE),
            ImGuiCond_Once);

          ImGuiWindowFlags pieceFlags = ImGuiWindowFlags_NoDecoration |
            ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollWithMouse;

          ImGui::Begin(dragDropManager.GetDragItemName().c_str(), NULL,
                       pieceFlags);

          auto piecePos = ImGui::GetWindowPos();
          ImGui::GetStyle().WindowBorderSize = 0.f;

          auto halfSquare = CUBE_FACE_SIZE / 2;
          dragDropManager.SetDragItemCurrCellId(
            int((piecePos.x + halfSquare) / CUBE_FACE_SIZE) +
            int((piecePos.y + halfSquare) / CUBE_FACE_SIZE) *
              NUMBER_OF_FIELD_COLUMNS);

          AddImageToPieceBackgroundDrawList(dragDropManager.GetDragPiece(),
                                            piecePos, textureStorage);

          ImGui::End();
        }
      }

      /// Draw wrong move errors
      if (!uiNotifications.empty()) {

        ImGui::SetNextWindowSize(ImVec2(375, 100), ImGuiCond_Once);

        ImGui::Begin("Error", NULL,
                     ImGuiWindowFlags_NoDecoration |
                       ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

        ImGui::SetWindowPos(
          ImVec2(io.DisplaySize.x / 4,
                 io.DisplaySize.y / 2 - ImGui::GetWindowHeight()),
          ImGuiCond_Once);

        ImVec2 buttonOkSize(30, 20);

        ImGui::NewLine();
        ImGui::Text(uiNotifications.front().data());
        ImGui::NewLine();
        ImGui::Indent(ImGui::GetWindowWidth() / 2 - buttonOkSize.x);

        if (ImGui::Button("ok", buttonOkSize))
          uiNotifications.pop();

        ImGui::End();
      }

      /// Draw offer to play with AI
      if (!turnControl.IsPlayerMadeChoice()) {

        ImGui::SetNextWindowSize(ImVec2(375, 100), ImGuiCond_Once);

        ImGui::Begin("Error", NULL,
                     ImGuiWindowFlags_NoDecoration |
                       ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

        ImGui::SetWindowPos(
          ImVec2(io.DisplaySize.x / 4,
                 io.DisplaySize.y / 2 - ImGui::GetWindowHeight()),
          ImGuiCond_Once);

        ImVec2 buttonSize(30, 20);

        ImGui::NewLine();
        ImGui::Text("\t\t\tYou wont play with AI?");
        ImGui::NewLine();
        ImGui::Indent(ImGui::GetWindowWidth() / 2 - (buttonSize.x * 2));

        if (ImGui::Button("Yas", buttonSize)) {
          startGameAndChoseRival(true);
        }
        ImGui::SameLine();
        if (ImGui::Button("No", buttonSize)) {
          startGameAndChoseRival(false);
        }

        ImGui::End();
      }

      ImGui::Render();
      glViewport(0, 0, io.DisplaySize.x, io.DisplaySize.y);
      glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
      glClear(GL_COLOR_BUFFER_BIT);

      ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
      SDL_GL_SwapWindow(window.window);
    }

    return Quit();

  } catch (const std::exception& e) {
    MessageBox(NULL, e.what(), NULL, MB_OK | MB_ICONERROR);
  }
};