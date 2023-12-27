#include "ChessEngine.h"

#include <sstream>
#include <stdexcept>

namespace Chess
{
	void ChessEngine::SendCommand(const std::string& command)
	{
		if (!IsWorked()) {
			throw std::runtime_error("Engine Process not run");
		}

		if (command.empty()) {
			return;
		}

		static constexpr char NEW_LINE_CHAR = '\n';

		WriteFile(_writeStdIn, command.data(), static_cast<DWORD>(command.size()), &_numberOfBytesRead, nullptr);
		WriteFile(_writeStdIn, &NEW_LINE_CHAR, 1, &_numberOfBytesRead, nullptr);
	}

	bool ChessEngine::IsReady()
	{
		SendCommand("isready");
		Sleep(100);

		std::ignore = GetAnswer();

		return GetLastAnswer().find("readyok") != std::string::npos;
	}

	bool ChessEngine::GetAnswer()
	{
		if (!IsWorked()) {
			throw std::runtime_error("Engine Process not run");
		}

		_buffer.fill(0);

		PeekNamedPipe(_readStdOut, _buffer.data(), BUFF_SIZE - 1, &_numberOfBytesRead, &_totalBytesAvailable, nullptr);

		if (_numberOfBytesRead == 0) {
			return false;
		}

		std::stringstream ss;

		do {
			_buffer.fill(0);

			ReadFile(_readStdOut, _buffer.data(), BUFF_SIZE - 1, &_numberOfBytesRead, nullptr);

			ss << _buffer.data();

			_answers.push(ss.str());

		} while (false /*_numberOfBytesRead >= BUFF_SIZE - 1*/);

		return true;
	}

	std::string ChessEngine::GetLastAnswer() const
	{
		return _answers.empty() ? "answers empty" : _answers.top();
	}

	bool ChessEngine::IsWorked() const
	{
		unsigned long exit;
		GetExitCodeProcess(_processInformation.hProcess, &exit);
		return exit == STILL_ACTIVE;
	}

	bool ChessEngine::IsWhiteTurnToMove() const
	{
		return _fen.find('w') != std::string::npos;
	}

	WinningSide ChessEngine::IsSomebodyWon()
	{
		SendCommand("go mate 1");
		Sleep(50);

		SendCommand("stop");
		Sleep(50);

		std::ignore = GetAnswer();

		if (GetLastAnswer().find("bestmove (none)") != std::string::npos) {
			return IsWhiteTurnToMove() ? WinningSide::Black : WinningSide::White;
		}

		return WinningSide::NoOne;
	}

	std::string ChessEngine::GetFen() const
	{
		return _fen;
	}

	void ChessEngine::UpdateFen()
	{
		std::ignore = GetAnswer();

		SendCommand("d");
		Sleep(100);

		std::ignore = GetAnswer();

		const std::string answer = GetLastAnswer();

		const size_t fenBeginIndex = answer.find("Fen: ") + 5;
		const size_t fenSize = answer.find("\r\n", fenBeginIndex) - fenBeginIndex;

		_fen = answer.substr(fenBeginIndex, fenSize);
	}

	ChessEngine::~ChessEngine()
	{
		SendCommand("quit");
		Sleep(500);

		TerminateProcess(_processInformation.hProcess, 0);
		CloseHandle(_processInformation.hThread);
		CloseHandle(_processInformation.hProcess);
		CloseHandle(_newStdIn);
		CloseHandle(_newStdOut);
		CloseHandle(_readStdOut);
		CloseHandle(_writeStdIn);
	}

	ChessEngine::ChessEngine(const std::string& enginePath)
	{
		_securityAttributes.lpSecurityDescriptor = nullptr;
		_securityAttributes.nLength = sizeof(SECURITY_ATTRIBUTES);
		_securityAttributes.bInheritHandle = true;

		if (!CreatePipe(&_newStdIn, &_writeStdIn, &_securityAttributes, 0)) {
			throw std::runtime_error("Error CreatePipe std::in!");
		}

		if (!CreatePipe(&_readStdOut, &_newStdOut, &_securityAttributes, 0)) {
			throw std::runtime_error("Error CreatePipe std::out!");
		}

		GetStartupInfo(&_startupInfo);

		_startupInfo.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
		_startupInfo.wShowWindow = SW_HIDE;
		_startupInfo.hStdOutput = _newStdOut;
		_startupInfo.hStdError = _newStdOut;
		_startupInfo.hStdInput = _newStdIn;

		if (!CreateProcess(enginePath.data(), nullptr, nullptr, nullptr, TRUE, CREATE_NEW_CONSOLE, nullptr, nullptr, &_startupInfo, &_processInformation)) {
			CloseHandle(_newStdIn);
			CloseHandle(_newStdOut);
			CloseHandle(_readStdOut);
			CloseHandle(_writeStdIn);
			throw std::runtime_error("Error CreateProcess!");
		}
	}
} // namespace Chess