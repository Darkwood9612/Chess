#include "DragDropManager.h"

void DragDropManager::SetDragItem(std::string name, int8_t x, int8_t y,
                                  char piece)
{
  if (this->dragItemName != name)
    StartDragCallback(x,y);

  this->dragItemName = name;
  this->dragItemStartPosY = y;
  this->dragItemStartPosX = x;
  this->dragItemCurrPosY = y;
  this->dragItemCurrPosX = x;
  this->piece = piece;
}

void DragDropManager::DropItem()
{
  StartDragCallback(this->dragItemCurrPosX, this->dragItemCurrPosY);
  this->dragItemName = " ";
  this->dragItemStartPosY = -1;
  this->dragItemStartPosX = -1;
  this->dragItemCurrPosY = -1;
  this->dragItemCurrPosX = -1;
  this->piece = '\0';
}

void DragDropManager::SetDragCallback(std::function<void(int8_t, int8_t)> fn)
{
  this->StartDragCallback = fn;
}

void DragDropManager::SetDropCallback(std::function<void(int8_t, int8_t)> fn)
{
  this->StartDropCallback = fn;
}
