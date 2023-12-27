#pragma once
#include "ChessDefines.h"

#include <array>
#include <stack>
#include <string>
#include <Windows.h>

namespace Chess
{
	class ChessEngine final
	{
	public:
		explicit ChessEngine(const std::string& enginePath);
		~ChessEngine();

		ChessEngine(const ChessEngine&) = delete;
		ChessEngine& operator=(const ChessEngine&) = delete;

		ChessEngine(ChessEngine&&) = delete;
		ChessEngine& operator=(ChessEngine&&) = delete;

	public:
		void SendCommand(const std::string& command);

		[[nodiscard]] bool IsReady();
		[[nodiscard]] bool IsWhiteTurnToMove() const;

		void UpdateFen();

		[[nodiscard]] WinningSide IsSomebodyWon();

		[[nodiscard]] std::string GetFen() const;

		[[nodiscard]] std::string GetLastAnswer() const;

	private:
		[[nodiscard]] bool IsWorked() const;
		[[nodiscard]] bool GetAnswer();

		std::string _fen {};
		std::stack<std::string> _answers {};

		static constexpr inline uint32_t BUFF_SIZE = 1024;

		std::array<char, BUFF_SIZE> _buffer {};

		unsigned long _numberOfBytesRead = 0;
		unsigned long _totalBytesAvailable = 0;

		STARTUPINFO _startupInfo;
		SECURITY_ATTRIBUTES _securityAttributes;
		PROCESS_INFORMATION _processInformation;
		HANDLE _newStdIn, _newStdOut, _readStdOut, _writeStdIn;
	};
} // namespace Chess