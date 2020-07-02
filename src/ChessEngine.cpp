#include "ChessEngine.h"
#include <sstream>
#include <stdexcept>

void ChessEngine::SendCommand(const std::string& command)
{
  if (!ChessEngineIsWork())
    return;

  static const char endl = '\n';

  WriteFile(write_stdin, command.data(), command.size(), &bread, NULL);
  WriteFile(write_stdin, &endl, 1, &bread, NULL);
}

void ChessEngine::SetOption(const std::string& name, const std::string& value)
{
  std::string command = "setoption name " + name + " value " + value;
  SendCommand(command);
}

void ChessEngine::Position(const std::string& fen)
{
  std::string command = "position fen " + fen;
  SendCommand(command);
}

void ChessEngine::Position(const std::string& begin, const std::string& end)
{
  std::string command = "position startpos moves " + begin + " " + end;
  SendCommand(command);
}

std::string ChessEngine::GetAnswer()
{
  if (!ChessEngineIsWork())
    return "ChessEngine process is closed";

  PeekNamedPipe(read_stdout, buf, BUFF_SIZE - 1, &bread, &avail, NULL);

  if (bread == 0)
    return "stdout empty";

  bzero(buf);
  std::stringstream ss;

  bool stdOutGeteerBuff = avail > BUFF_SIZE - 1;

  if (stdOutGeteerBuff) {

    while (bread >= BUFF_SIZE - 1) {
      ReadFile(read_stdout, buf, BUFF_SIZE - 1, &bread, NULL);
      ss << buf;
      bzero(buf);
    }
    return ss.str();
  }

  ReadFile(read_stdout, buf, BUFF_SIZE - 1, &bread, NULL);
  ss << buf;
  return ss.str();
}

bool ChessEngine::ChessEngineIsWork()
{
  unsigned long exit = 0;
  GetExitCodeProcess(pi.hProcess, &exit);
  return exit == STILL_ACTIVE;
}

ChessEngine::~ChessEngine()
{
  CloseHandle(pi.hThread);
  CloseHandle(pi.hProcess);
  CloseHandle(newstdin);
  CloseHandle(newstdout);
  CloseHandle(read_stdout);
  CloseHandle(write_stdin);
}

void ChessEngine::Init(const std::string& enginePath)
{
  sa.lpSecurityDescriptor = NULL;
  sa.nLength = sizeof(SECURITY_ATTRIBUTES);
  sa.bInheritHandle = true;

  if (!CreatePipe(&newstdin, &write_stdin, &sa, 0))
    throw std::runtime_error("error CreatePipe stdin");

  if (!CreatePipe(&read_stdout, &newstdout, &sa, 0)) {
    CloseHandle(newstdin);
    CloseHandle(write_stdin);
    throw std::runtime_error("error CreatePipe stdout");
  }

  GetStartupInfo(&si);

  si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
  si.wShowWindow = SW_HIDE;
  si.hStdOutput = newstdout;
  si.hStdError = newstdout;
  si.hStdInput = newstdin;

  if (!CreateProcess(enginePath.data(), NULL, NULL, NULL, TRUE,
                     CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi)) {
    CloseHandle(newstdin);
    CloseHandle(newstdout);
    CloseHandle(read_stdout);
    CloseHandle(write_stdin);
    throw std::runtime_error("error CreateProcess");
  }

  bzero(buf);

  SendCommand("uci");
}
