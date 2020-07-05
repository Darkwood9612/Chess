#pragma once
#include "ChessDefines.h"
#include "GameModel.h"
#include <array>
#include <string>
#include <vector>

namespace util {

void split(std::string& str, std::vector<std::string>& result,
           const char* sep);
void FennToArray(const std::string& fen, std::array<Game::Piece, 64>& array, bool nowWhiteMove);

}