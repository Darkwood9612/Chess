#pragma once
#include "ChessDefines.h"
#include <array>
#include "ChessEngine.h"

class Game
{
public:
  struct Piece
  {
    bool isMovable = false;
    EPiece type = EPiece::EMPTY;
  };

  void StartNewGame();
  std::string GetFEN(bool* isChanged = nullptr);

  Game(ChessEngine* engine) { this->engine = engine; };

  const std::array<Piece, 64>& GetFieldData() { return field; };
  void UpdateField(const std::string& fen);
  void UpdateField();
  bool NowWhiteMove();

private:
  std::string UpdateFEN();
  bool IsFEN_Changed(const std::string& fen);

  std::string FEN;

  ChessEngine* engine = nullptr;
  std::array<Piece, 64> field;
};
