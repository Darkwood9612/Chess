#include "DragDropManager.h"

namespace Chess
{
	void DragDropManager::SetDragItem(const int32_t cellNumber, const int32_t x, const int32_t y, const EPiece pieceType)
	{
		if (_currentDraggedCellNumber == cellNumber) {
			return;
		}

		_currentDraggedCellNumber = cellNumber;
		_currentOverlappedCellNumber = cellNumber;

		_dragItemStartPosX = x;
		_dragItemStartPosY = y;

		_currentDraggedPieceType = pieceType;

		if (_startDragCallback) {
			_startDragCallback(_currentDraggedCellNumber);
		}
	}

	bool DragDropManager::HasDraggedPiece() const noexcept
	{
		return _currentDraggedPieceType != EPiece::EMPTY && _currentDraggedCellNumber >= 0;
	}

	void DragDropManager::DropDraggedPiece(const bool isAi)
	{
		const int32_t currentOverlappedCellNumber = _currentOverlappedCellNumber;

		_currentDraggedCellNumber = -1;
		_currentOverlappedCellNumber = -1;

		_dragItemStartPosX = -1;
		_dragItemStartPosY = -1;

		_currentDraggedPieceType = EPiece::EMPTY;

		if (_startDropCallback) {
			_startDropCallback(isAi, currentOverlappedCellNumber);
		}
	}

	int32_t DragDropManager::GetCurrentDraggedCellNumber() const noexcept
	{
		return _currentDraggedCellNumber;
	}

	int32_t DragDropManager::GetCurrentOverlappedCellId() const noexcept
	{
		return _currentOverlappedCellNumber;
	}

	EPiece DragDropManager::GetCurrentDraggedPieceType() const noexcept
	{
		return _currentDraggedPieceType;
	}

	void DragDropManager::SetCurrentOverlappedCellId(const int32_t id)
	{
		_currentOverlappedCellNumber = id;
	}

	int32_t DragDropManager::GetDragItemStartPosX() const noexcept
	{
		return _dragItemStartPosX;
	}

	int32_t DragDropManager::GetDragItemStartPosY() const noexcept
	{
		return _dragItemStartPosY;
	}

	void DragDropManager::SetDragCallback(DragCallback fn)
	{
		_startDragCallback = std::move(fn);
	}

	void DragDropManager::SetDropCallback(DropCallback fn)
	{
		_startDropCallback = std::move(fn);
	}
} // namespace Chess