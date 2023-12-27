#include "Move.h"

#include <stdexcept>

namespace Chess
{
	void Move::SetFrom(const int32_t cellId, const int32_t numColumns)
	{
		_from = IdToBoardPos(cellId, numColumns);
	}

	void Move::SetTo(const int32_t cellId, const int32_t numColumns)
	{
		_to = IdToBoardPos(cellId, numColumns);
	}

	void Move::SetMove(const std::string& move)
	{
		if (move.size() < 4) {
			throw std::runtime_error("move.size() < 4");
		}

		_from = move.substr(0, 2);
		_to = move.substr(2);
	}

	std::string Move::GetMove() const
	{
		return _from + _to;
	}

	bool Move::IsFromEqualTo() const noexcept
	{
		return _from == _to;
	}

	bool Move::IsValidCellId(const int32_t cellId, const int32_t numColumns)
	{
		return numColumns * numColumns > cellId && cellId >= 0;
	}

	std::string Move::IdToBoardPos(const int32_t cellId, const int32_t numColumns)
	{
		if (!IsValidCellId(cellId, numColumns)) {
			return {};
		}

		const int32_t y = numColumns - cellId / numColumns;
		const int32_t x = cellId - numColumns * (cellId / numColumns);

		std::string res;

		constexpr char a = 'a';

		res = static_cast<char>(a + x);
		res += std::to_string(y);

		return res;
	}

	void TurnControl::SetPlayerMadeChoice(const bool playerWontPlayWithAi)
	{
		_playerPlayWithAi = _playerMadeChoice ? _playerPlayWithAi : playerWontPlayWithAi;

		_playerMadeChoice = true;
	}

	void TurnControl::SetGameOver(const WinningSide winner)
	{
		_currentWinner = { true, winner };
	}

	bool TurnControl::IsEngineThoughtEnough() const noexcept
	{
		return (std::chrono::high_resolution_clock::now() - _startedToThink).count() >= RUNNING_TIME;
	}

	bool TurnControl::IsGameOver() const noexcept
	{
		return _currentWinner.first;
	}

	bool TurnControl::IsPlayerMadeChoice() const noexcept
	{
		return _playerMadeChoice;
	}

	bool TurnControl::IsPlayerPlayWithAI() const noexcept
	{
		return _playerPlayWithAi;
	}

	void TurnControl::StartNewGame()
	{
		_playerMadeChoice = false;
		_currentWinner = { false, WinningSide::NoOne };
		_startedToThink = std::chrono::high_resolution_clock::now();
	}

	void TurnControl::SetStartThinkTime()
	{
		_startedToThink = std::chrono::high_resolution_clock::now();
	}
} // namespace Chess