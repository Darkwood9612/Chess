#pragma once
#include <string>

struct Move
{
public:
  void SetFrom(int8_t cellId, uint8_t numColumns);
  void SetTo(int8_t cellId, uint8_t numColumns);
  std::string GetMove() { return from+to; };

private:
  bool IsValidCellId(int8_t cellId, uint8_t numColumns);
  std::string IdToBoardPos(int8_t cellId, uint8_t numColumns);
  std::string from;
  std::string to;
};