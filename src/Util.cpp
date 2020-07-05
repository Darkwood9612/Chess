#include "util.h"

namespace {
void FillArrayEMPTY(std::array<char, 64>& array, size_t startPos,
                    size_t numItems)
{
  for (size_t i = startPos; i < startPos + numItems; ++i) {
    array[i] = '\0';
  }
};
}

namespace util {

void split(std::string& s, std::vector<std::string>& fields, const char* sep)
{
  fields.clear();
  size_t next = 0;
  bool more = true;

  while (more && s.length() > next) {
    size_t offset = s.find(sep, next);
    std::string frag;

    if (offset != std::string::npos)
      frag = s.substr(next, offset - next);
    else {
      frag = s.substr(next);
      more = false;
    }

    fields.push_back(frag);
    if (more)
      next = offset + 1;
  }
}
std::array<char, 64> FennToArray(const std::string& fen)
{
  size_t charIndex = 0;
  std::array<char, 64> result;

  static const std::string pieces = "rnbqkpPRNBQK";
  static const std::string numbers = "12345678";

  for (size_t i = 0; i < result.size(); ++i) {

    if (pieces.find(fen[charIndex]) != std::string::npos) {
      result[i] = fen[charIndex];
      ++charIndex;
      continue;
    }

    if (numbers.find(fen[charIndex]) != std::string::npos) {
      uint8_t number = atoi(&fen[charIndex]);
      FillArrayEMPTY(result, i, number);
      ++charIndex;
      i += (number - 1);
      continue;
    }

    if (fen[charIndex] == '/') {
      --i;
      ++charIndex;
    }
  }
  return result;
}
}