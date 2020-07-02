#pragma once
#include <Windows.h>
#include <string>

#define bzero(a) memset(a, 0, sizeof(a))

class ChessEngine
{
public:
  ChessEngine(const std::string& enginePath) { Init(enginePath); };

  void SendCommand(const std::string& command);
  void SetOption(const std::string& name, const std::string& value);
  void Position(const std::string& begin, const std::string& end);
  void Position(const std::string& fen);

  std::string GetAnswer();
  bool ChessEngineIsWork();

  ~ChessEngine();

private:
  void Init(const std::string& enginePath);

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