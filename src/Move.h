#pragma once
#include "ChessDefines.h"

#include <chrono>
#include <string>

namespace Chess
{

	struct Move
	{
	public:
		void SetFrom(int32_t cellId, int32_t numColumns);
		void SetTo(int32_t cellId, int32_t numColumns);
		void SetMove(const std::string& move);

		[[nodiscard]] std::string GetMove() const;
		[[nodiscard]] bool IsFromEqualTo() const noexcept;

		[[nodiscard]] static bool IsValidCellId(int32_t cellId, int32_t numColumns);
		[[nodiscard]] static std::string IdToBoardPos(int32_t cellId, int32_t numColumns);

	private:
		std::string _from;
		std::string _to;
	};

	struct TurnControl
	{
	public:
		void StartNewGame();
		void SetStartThinkTime();
		void SetPlayerMadeChoice(bool playerWontPlayWithAi);

		void SetGameOver(WinningSide winner);

		[[nodiscard]] bool IsGameOver() const noexcept;
		[[nodiscard]] bool IsPlayerMadeChoice() const noexcept;
		[[nodiscard]] bool IsPlayerPlayWithAI() const noexcept;
		[[nodiscard]] bool IsEngineThoughtEnough() const noexcept;

	private:
		bool _playerMadeChoice { false };
		bool _playerPlayWithAi { false };

		std::pair<bool, WinningSide> _currentWinner { false, WinningSide::NoOne };

		static constexpr inline int RUNNING_TIME { 3000 };

		std::chrono::time_point<std::chrono::steady_clock> _startedToThink {};
	};
} // namespace Chess