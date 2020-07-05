#include "GameModel.h"
#include "Util.h"

void Game::StartNewGame()
{
  if (!engine)
    throw std::runtime_error("engine == nullptr");

  engine->SendCommand("uci");

  if (engine->IsReady())
    engine->SendCommand("ucinewgame");
}

std::string Game::GetFEN(bool* isChanged)
{
  auto newFEN = UpdateFEN();
  bool isDifferent = IsFEN_Changed(newFEN);

  if (isChanged)
    *isChanged = isDifferent;

  this->FEN = isDifferent ? newFEN : this->FEN;
  return this->FEN;
}

void Game::UpdateField(const std::string& fen)
{
  util::FennToArray(fen, field, NowWhiteMove());
}

void Game::UpdateField()
{
  UpdateField(GetFEN());
}

bool Game::NowWhiteMove()
{
  auto i = this->FEN.find('w');
  return i != std::string::npos;
}

std::string Game::UpdateFEN()
{
  if (!engine)
    throw std::runtime_error("engine == nullptr");
  
  engine->HasAnswer();
  engine->SendCommand("d");
  
  bool hasAnswer = false;
  
  while (!hasAnswer) {
    hasAnswer = engine->HasAnswer();
    Sleep(50);
  }
  
  std::string parsAnswer = engine->GetLastAnswer();
  size_t fenBegin =  parsAnswer.find("Fen: ") + 5;
  size_t fenEnd = parsAnswer.find("\r\n", fenBegin) - fenBegin;
  const char* source = parsAnswer.data() + fenBegin;
  char* str = new char[fenEnd];
  strncpy(str, source, fenEnd);
  std::string result = str;
  
  if (result.size() > fenEnd)
    result.erase(fenEnd, result.size() - fenEnd);
  
  return result;
}

bool Game::IsFEN_Changed(const std::string& fen)
{
  return !!strcmp(this->FEN.c_str(), fen.c_str());
}
