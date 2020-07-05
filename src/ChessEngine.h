#pragma once
#include <Windows.h>
#include <array>
#include <string>
#include <vector>

class ChessEngine
{
public:
  ChessEngine(const std::string& enginePath);
  ~ChessEngine();

  void StartNewGame();

  void SendCommand(const std::string& command);
  bool IsReady();
  bool NowWhiteMove();

  std::string GetFen() { return Fen; };
  void UpdateFen();

  const std::vector<std::string>& GetAnswers()
  {
    return answers;
  }; /// TODO Delete

private:
  bool IsWorked(unsigned long& exit);
  std::string GetLastAnswer();
  bool GetAnswer();
  std::vector<std::string> answers;

  std::string Fen;

  static const uint32_t BUFF_SIZE = 1024;
  std::array<char, BUFF_SIZE> buf;

  unsigned long bread = 0;
  unsigned long avail = 0;

  STARTUPINFO si;
  SECURITY_ATTRIBUTES sa;
  PROCESS_INFORMATION pi;

  HANDLE newstdin, newstdout, read_stdout, write_stdin;
};