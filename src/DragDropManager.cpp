#include "DragDropManager.h"

void DragDropManager::SetDragItem(std::string name, int8_t x, int8_t y,
                                  uint8_t numColumns, char _piece)
{
  dragItemCurrCellId = x + y * numColumns;
  dragItemStartPosY = y;
  dragItemStartPosX = x;
  piece = _piece;

  if (dragItemName != name)
    StartDragCallback(dragItemCurrCellId);

  dragItemName = name;
}

void DragDropManager::DropItem()
{
  StartDropCallback(false, dragItemCurrCellId);
  dragItemName = "null";
  dragItemStartPosY = -1;
  dragItemStartPosX = -1;
  dragItemCurrCellId = -1;
  piece = '\0';
}

void DragDropManager::DropItemAI()
{
  StartDropCallback(true, dragItemCurrCellId);
}

void DragDropManager::SetDragCallback(
  std::function<void(int8_t)> fn)
{
  StartDragCallback = fn;
}

void DragDropManager::SetDropCallback(std::function<void(bool, int8_t)> fn)
{
  StartDropCallback = fn;
}
