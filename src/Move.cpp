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
