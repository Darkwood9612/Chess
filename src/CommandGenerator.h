#pragma once
#include <string>

class CommandGenerator
{
public:
  static std::string NewGame();

  static std::string Position(std::string fen);
  static std::string Position(std::string fen, std::string moves);
  static std::string GoSearchInfinite();
  static std::string Stop();
};
