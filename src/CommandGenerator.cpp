#include "CommandGenerator.h"

std::string CommandGenerator::NewGame()
{
  return "ucinewgame";
}

std::string CommandGenerator::Position(std::string fen)
{
  return "position fen " + fen;
}

std::string CommandGenerator::Position(std::string fen, std::string moves)
{
  return "position fen " + fen + " moves " + moves;
}
