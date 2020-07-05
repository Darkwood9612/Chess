#pragma once
#include <array>
#include <string>
#include <vector>

namespace util {

void split(std::string& str, std::vector<std::string>& result,
           const char* sep);

std::array<char, 64> FennToArray(const std::string& fen);

}