#include "Move.h"
#include <stdexcept>

void Move::SetFrom(int8_t cellId, uint8_t numColumns)
{
  from = IdToBoardPos(cellId, numColumns);
}

void Move::SetTo(int8_t cellId, uint8_t numColumns)
{
  to = IdToBoardPos(cellId, numColumns);
}

void Move::SetMove(std::string move)
{
  if (move.size() < 4)
    throw std::runtime_error("move.size() < 4");

  from = move[0];
  from += move[1];
  to = move[2];
  to += move[3];
}

bool Move::IsValidCellId(int8_t cellId, uint8_t numColumns)
{
  return numColumns * numColumns > cellId && cellId >= 0;
}

std::string Move::IdToBoardPos(int8_t cellId, uint8_t numColumns)
{
  if (!IsValidCellId(cellId, numColumns))
    throw std::runtime_error("Cell Id not walid. Id = " +
                             std::to_string(cellId));

  uint8_t y = numColumns - cellId / numColumns;
  uint8_t x = cellId - numColumns * (cellId / numColumns);

  std::string res;
  res = 'a' + x;
  res += std::to_string(y);

  return res;
}

void TurnControl::SetPlayerMadeChoice(bool playerWontPlayWithAI)
{
  playerPlayWithAI =
    playerMadeChoice.has_value() ? playerPlayWithAI : playerWontPlayWithAI;
  playerMadeChoice = playerMadeChoice.has_value() ? playerMadeChoice : true;
}

bool TurnControl::IsEngineThoughtEnough()
{
  return (clock() - startedToThink) > runningTime;
}

void TurnControl::StartNewGame()
{
  gameOver = false;
  startedToThink = 0;
  playerMadeChoice.reset();
}
