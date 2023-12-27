#include "CommandGenerator.h"

namespace Chess
{
	std::string CommandGenerator::NewGame()
	{
		return "position startpos";
	}

	std::string CommandGenerator::Position(std::string fen)
	{
		return "position fen " + fen;
	}

	std::string CommandGenerator::Position(std::string fen, std::string moves)
	{
		return "position fen " + fen + " moves " + moves;
	}

	std::string CommandGenerator::GoSearchInfinite()
	{
		return "go infinite";
	}

	std::string CommandGenerator::Stop()
	{
		return "stop";
	}
} // namespace Chess