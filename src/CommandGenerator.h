#pragma once
#include <string>

namespace Chess
{
	class CommandGenerator
	{
	public:
		static std::string NewGame();

		static std::string Position(std::string fen);
		static std::string Position(std::string fen, std::string moves);
		static std::string GoSearchInfinite();
		static std::string Stop();
	};
} // namespace Chess