#pragma once
#include <Windows.h>
#include <string>
#include <vector>

#define bzero(a) memset(a, 0, sizeof(a))

class ChessEngine
{
public:
  ChessEngine(const std::string& enginePath) { Init(enginePath); };

  void SendCommand(const std::string& command);
  void SetOption(const std::string& name, const std::string& value);
  void PositionStartPos(const std::string& moves);
  void Position(const std::string& fen);
  void Position(const std::string& fen, const std::string& moves);

  bool IsReady();

  bool HasAnswer();

  const std::vector<std::string>& GetAnswers() { return answers; };
  std::string GetLastAnswer();
  bool ChessEngineIsWork();

  ~ChessEngine();

private:
  void Init(const std::string& enginePath);

  std::vector<std::string> answers;

  static const uint32_t BUFF_SIZE = 1024;
  char buf[BUFF_SIZE]; //буфер ввода/вывода

  unsigned long bread; //кол-во прочитанных байт
  unsigned long avail; //кол-во доступных байт

  STARTUPINFO si;
  SECURITY_ATTRIBUTES sa;
  SECURITY_DESCRIPTOR sd;
  PROCESS_INFORMATION pi;

  HANDLE newstdin, newstdout, read_stdout, write_stdin;
};