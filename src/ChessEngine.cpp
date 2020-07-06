#include "ChessEngine.h"
#include <sstream>
#include <stdexcept>

void ChessEngine::SendCommand(const std::string& command)
{
  if (!IsWorked())
    throw std::runtime_error("Engine Process not run");

  static const char endl = '\n';

  WriteFile(write_stdin, command.data(), command.size(), &bread, NULL);
  WriteFile(write_stdin, &endl, 1, &bread, NULL);
}

bool ChessEngine::IsReady()
{
  SendCommand("isready");
  Sleep(100);
  GetAnswer();
  return GetLastAnswer().find("readyok") != std::string::npos;
}

bool ChessEngine::GetAnswer()
{

  if (!IsWorked())
    throw std::runtime_error("Engine Process not run");

  PeekNamedPipe(read_stdout, buf.data(), BUFF_SIZE - 1, &bread, &avail, NULL);

  if (bread == 0)
    return false;

  buf.fill(0);
  std::stringstream ss;

  bool stdOutGeteerBuff = avail > BUFF_SIZE - 1;

  if (stdOutGeteerBuff) {

    while (bread >= BUFF_SIZE - 1) {
      ReadFile(read_stdout, buf.data(), BUFF_SIZE - 1, &bread, NULL);
      ss << buf.data();
      buf.fill(0);
    }
    answers.push(ss.str());
    return true;
  }

  ReadFile(read_stdout, buf.data(), BUFF_SIZE - 1, &bread, NULL);
  ss << buf.data();
  answers.push(ss.str());
  return true;
}

std::string ChessEngine::GetLastAnswer()
{
  if (answers.empty())
    return "answers empty";

  return answers.top();
}

bool ChessEngine::IsWorked()
{
  unsigned long exit;
  GetExitCodeProcess(pi.hProcess, &exit);
  return exit == STILL_ACTIVE;
}

bool ChessEngine::NowWhiteMove()
{
  return this->Fen.find('w') != std::string::npos;
}

ChessEngine::WinningSide ChessEngine::IsSomebodyWon()
{
  SendCommand("go mate 1");
  Sleep(50);
  SendCommand("stop");
  Sleep(50);

  if (auto answer = GetAnswer() &&
        GetLastAnswer().find("bestmove (none)") != std::string::npos)
    return NowWhiteMove() ? ChessEngine::WinningSide::Black
                          : ChessEngine::WinningSide::White;

  return ChessEngine::WinningSide::NoOne;
}

void ChessEngine::UpdateFen()
{
  GetAnswer();
  SendCommand("d");
  Sleep(100);
  GetAnswer();

  std::string answer = GetLastAnswer();

  size_t fenBeginIndex = answer.find("Fen: ") + 5;
  size_t fenSize = answer.find("\r\n", fenBeginIndex) - fenBeginIndex;

  const char* fenPtr = answer.data() + fenBeginIndex;
  char* resPrt = new char[fenSize];
  strncpy(resPrt, fenPtr, fenSize);
  std::string result = resPrt;

  if (result.size() > fenSize)
    result.erase(fenSize, result.size() - fenSize);

  result += '\0';
  this->Fen = result;
}

ChessEngine::~ChessEngine()
{
  SendCommand("quit");
  Sleep(500);
  CloseHandle(pi.hThread);
  CloseHandle(pi.hProcess);
  CloseHandle(newstdin);
  CloseHandle(newstdout);
  CloseHandle(read_stdout);
  CloseHandle(write_stdin);

  if (IsWorked())
    TerminateProcess(pi.hProcess, 0);
}

void ChessEngine::StartNewGame()
{
  while (!answers.empty())
    answers.pop();

  SendCommand("uci");
  Sleep(100);
  GetAnswer();
  SendCommand("ucinewgame");
  Sleep(200);
  GetAnswer();
}

ChessEngine::ChessEngine(const std::string& enginePath)
{
  sa.lpSecurityDescriptor = NULL;
  sa.nLength = sizeof(SECURITY_ATTRIBUTES);
  sa.bInheritHandle = true;

  if (!CreatePipe(&newstdin, &write_stdin, &sa, 0))
    throw std::runtime_error("Error CreatePipe stdin!");

  if (!CreatePipe(&read_stdout, &newstdout, &sa, 0)) {
    CloseHandle(newstdin);
    CloseHandle(write_stdin);
    throw std::runtime_error("Error CreatePipe stdout!");
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
    throw std::runtime_error("Error CreateProcess!");
  }

  buf.fill(0);
}
