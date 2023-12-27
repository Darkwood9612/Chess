#pragma once

namespace Chess
{
	enum class WinningSide
	{
		NoOne = 0,
		White,
		Black
	};

	enum class EPiece : char
	{
		EMPTY = '\0',
		B_PAWN = 'p',
		B_KNIGHT = 'n',
		B_BISHOP = 'b',
		B_ROOK = 'r',
		B_QUEEN = 'q',
		B_KING = 'k',
		W_PAWN = 'P',
		W_KNIGHT = 'N',
		W_BISHOP = 'B',
		W_ROOK = 'R',
		W_QUEEN = 'Q',
		W_KING = 'K',
	};

	inline bool IsWhite(const EPiece piece)
	{
		return piece == EPiece::W_PAWN || piece == EPiece::W_KNIGHT || piece == EPiece::W_BISHOP || piece == EPiece::W_ROOK || piece == EPiece::W_QUEEN
			   || piece == EPiece::W_KING;
	}

	inline bool IsBlack(const EPiece piece)
	{
		return piece == EPiece::B_PAWN || piece == EPiece::B_KNIGHT || piece == EPiece::B_BISHOP || piece == EPiece::B_ROOK || piece == EPiece::B_QUEEN
			   || piece == EPiece::B_KING;
	}

	inline bool IsEmpty(const EPiece piece)
	{
		return piece == EPiece::EMPTY;
	}
} // namespace Chess