#pragma once
#include "ChessDefines.h"

#include <cstdint>
#include <functional>

namespace Chess
{

	class DragDropManager
	{
	public:
		using DragCallback = std::function<void(int32_t)>;
		using DropCallback = std::function<void(bool, int32_t)>;

	public:
		void DropDraggedPiece(bool isAi = false);

		void SetCurrentOverlappedCellId(int32_t id);

		void SetDragItem(int32_t cellNumber, int32_t x, int32_t y, EPiece pieceType);

		[[nodiscard]] bool HasDraggedPiece() const noexcept;

		[[nodiscard]] int32_t GetCurrentDraggedCellNumber() const noexcept;

		[[nodiscard]] EPiece GetCurrentDraggedPieceType() const noexcept;
		[[nodiscard]] int32_t GetCurrentOverlappedCellId() const noexcept;

		[[nodiscard]] int32_t GetDragItemStartPosX() const noexcept;
		[[nodiscard]] int32_t GetDragItemStartPosY() const noexcept;

		void SetDragCallback(DragCallback fn);
		void SetDropCallback(DropCallback fn);

	private:
		int32_t _currentDraggedCellNumber { -1 };
		int32_t _currentOverlappedCellNumber { -1 };

		int32_t _dragItemStartPosX { -1 };
		int32_t _dragItemStartPosY { -1 };

		EPiece _currentDraggedPieceType { EPiece::EMPTY };

		DragCallback _startDragCallback {};
		DropCallback _startDropCallback {};
	};
} // namespace Chess