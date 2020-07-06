#pragma once
#include <Windows.h>
#include <array>
#include <stack>
#include <string>

class ChessEngine
{
public:
  enum class WinningSide
  {
    NoOne = 1,
    White,
    Black
  };

  ChessEngine(const std::string& enginePath);
  ~ChessEngine();

  void StartNewGame();
  void SendCommand(const std::string& command);

  bool IsReady();
  bool NowWhiteMove();
  void UpdateFen();

  WinningSide IsSomebodyWon();
  std::string GetFen() { return Fen; };

private:
  bool IsWorked();

  std::string GetLastAnswer();
  bool GetAnswer();

  std::string Fen;
  std::stack<std::string> answers;
  static const uint32_t BUFF_SIZE = 1024;
  std::array<char, BUFF_SIZE> buf;

  unsigned long bread = 0;
  unsigned long avail = 0;

  STARTUPINFO si;
  SECURITY_ATTRIBUTES sa;
  PROCESS_INFORMATION pi;

  HANDLE newstdin, newstdout, read_stdout, write_stdin;
};