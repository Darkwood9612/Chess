#pragma once
#include <optional>
#include <string>
#include <ctime>

struct Move
{
public:
  void SetFrom(int8_t cellId, uint8_t numColumns);
  void SetTo(int8_t cellId, uint8_t numColumns);
  void SetMove(std::string move);

  std::string GetMove() { return from + to; };
  bool IsFromEqualTo() { return from == to; };

private:
  bool IsValidCellId(int8_t cellId, uint8_t numColumns);
  std::string IdToBoardPos(int8_t cellId, uint8_t numColumns);
  std::string from;
  std::string to;
};

struct TurnControl
{
public:
  bool IsPlayerPlayWithAI() { return playerPlayWithAI; };

  void SetPlayerMadeChoice(bool playerWontPlayWithAI);
  bool IsPlayerMadeChoice() { return playerMadeChoice.has_value(); };

  void SetStartThinkTime() { startedToThink = clock(); };
  bool IsEngineThoughtEnough();

  bool IsGameOver() { return gameOver; };
  void StartNewGame();

private:
  std::optional<bool> playerMadeChoice;
  const int runningTime = 3000;
  clock_t startedToThink = 0;
  bool playerPlayWithAI = false;
  bool gameOver = false;
};
