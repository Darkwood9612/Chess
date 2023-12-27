#pragma once
#include <array>
#include <string>
#include <vector>
#include "ChessDefines.h"

namespace utils {

void split(std::string& str, std::vector<std::string>& result,
           const char* sep);

std::array<Chess::EPiece, 64> FennToArray(const std::string& fen);

}