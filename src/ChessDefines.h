#pragma once
#include <cstdint>

enum class EPiece : char
{
  EMPTY = '\0',
  B_PAWN = 'p',
  B_KNIGHT = 'n',
  B_BISHOP = 'b',
  B_ROOK = 'r',
  B_QUEEN = 'q',
  B_KING = 'k',
  W_PAWN = 'P',
  W_KNIGHT = 'N',
  W_BISHOP = 'B',
  W_ROOK = 'R',
  W_QUEEN = 'Q',
  W_KING = 'K',
};

namespace {
bool IsWhite(char piece)
{
  return (EPiece)piece == EPiece::W_PAWN ||
    (EPiece)piece == EPiece::W_KNIGHT || (EPiece)piece == EPiece::W_BISHOP ||
    (EPiece)piece == EPiece::W_ROOK || (EPiece)piece == EPiece::W_QUEEN ||
    (EPiece)piece == EPiece::W_KING;
}
bool IsBlack(char piece)
{
  return (EPiece)piece == EPiece::B_PAWN ||
    (EPiece)piece == EPiece::B_KNIGHT || (EPiece)piece == EPiece::B_BISHOP ||
    (EPiece)piece == EPiece::B_ROOK || (EPiece)piece == EPiece::B_QUEEN ||
    (EPiece)piece == EPiece::B_KING;
}
bool IsEmpty(char piece)
{
  return (EPiece)piece == EPiece::EMPTY;
}
}
