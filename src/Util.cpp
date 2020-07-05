#include "util.h"

namespace {
void FillArrayEMPTY(std::array<Game::Piece, 64>& array, size_t startPos,
               size_t numItems)
{
  for (size_t i = startPos; i < startPos + numItems; ++i) {
    array[i].type = EPiece::EMPTY;
    array[i].isMovable = false;
  }
};
}

namespace util {

void split(std::string& s, std::vector<std::string>& fields,const char* sep)
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
void FennToArray(const std::string& fen, std::array<Game::Piece, 64>& array,
                 bool nowWhiteMove)
{
  size_t charIndex = 0;

  for (size_t i = 0; i < array.size(); ++i) {
    switch (fen.at(charIndex)) {
      case 'p':
        array[i].type = EPiece::B_PAWN;
        array[i].isMovable = !nowWhiteMove;
        ++charIndex;
        break;
      case 'r':
        array[i].type = EPiece::B_ROOK;
        array[i].isMovable = !nowWhiteMove;
        ++charIndex;
        break;
      case 'n':
        array[i].type = EPiece::B_KNIGHT;
        array[i].isMovable = !nowWhiteMove;
        ++charIndex;
        break;
      case 'b':
        array[i].type = EPiece::B_BISHOP;
        array[i].isMovable = !nowWhiteMove;
        ++charIndex;
        break;
      case 'q':
        array[i].type = EPiece::B_QUEEN;
        array[i].isMovable = !nowWhiteMove;
        ++charIndex;
        break;
      case 'k':
        array[i].type = EPiece::B_KING;
        array[i].isMovable = !nowWhiteMove;
        ++charIndex;
        break;
      case 'P':
        array[i].type = EPiece::W_PAWN;
        array[i].isMovable = nowWhiteMove;
        ++charIndex;
        break;
      case 'R':
        array[i].type = EPiece::W_ROOK;
        array[i].isMovable = nowWhiteMove;
        ++charIndex;
        break;
      case 'N':
        array[i].type = EPiece::W_KNIGHT;
        array[i].isMovable = nowWhiteMove;
        ++charIndex;
        break;
      case 'B':
        array[i].type = EPiece::W_BISHOP;
        array[i].isMovable = nowWhiteMove;
        ++charIndex;
        break;
      case 'Q':
        array[i].type = EPiece::W_QUEEN;
        array[i].isMovable = nowWhiteMove;
        ++charIndex;
        break;
      case 'K':
        array[i].type = EPiece::W_KING;
        array[i].isMovable = nowWhiteMove;
        ++charIndex;
        break;
      case '1':
        array[i].type = EPiece::EMPTY;
        array[i].isMovable = false;
        ++charIndex;
        break;
      case '2':
        FillArrayEMPTY(array, i, 2);
        i += 1;
        ++charIndex;
        break;
      case '3':
        FillArrayEMPTY(array, i, 3);
        i += 2;
        ++charIndex;
        break;
      case '4':
        FillArrayEMPTY(array, i, 4);
        i += 3;
        ++charIndex;
        break;
      case '5':
        FillArrayEMPTY(array, i, 5);
        i += 4;
        ++charIndex;
        break;
      case '6':
        FillArrayEMPTY(array, i, 6);
        i += 5;
        ++charIndex;
        break;
      case '7':
        FillArrayEMPTY(array, i, 7);
        i += 6;
        ++charIndex;
        break;
      case '8':
        FillArrayEMPTY(array, i, 8);
        i += 7;
        ++charIndex;
        break;
      case '/':
        --i;
        ++charIndex;
        break;
    }
  }
}
}