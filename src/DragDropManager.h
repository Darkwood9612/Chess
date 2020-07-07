#pragma once
#include <cstdint>
#include <functional>
#include <string>

class DragDropManager
{
public:
  void SetDragItem(std::string name, int8_t x, int8_t y, uint8_t numColumns,
                   char piece);
  void DropItem();
  void DropItemAI();

  std::string GetDragItemName() { return dragItemName; };
  char GetDragPiece() { return piece; };

  int8_t GetDragItemCurrCellId() { return dragItemCurrCellId; };
  void SetDragItemCurrCellId(int8_t id) { dragItemCurrCellId = id; };

  int8_t GetDragItemStartPosX() { return dragItemStartPosX; };
  int8_t GetDragItemStartPosY() { return dragItemStartPosY; };

  void SetDragCallback(std::function<void(int8_t)> fn);
  void SetDropCallback(std::function<void(bool , int8_t)> fn);

  bool IsEmpty() { return dragItemName == "null"; };

private:
  std::function<void(int8_t)> StartDragCallback;
  std::function<void(bool, int8_t)> StartDropCallback;

  std::string dragItemName = "null";
  int8_t dragItemCurrCellId = -1;

  int8_t dragItemStartPosX = -1;
  int8_t dragItemStartPosY = -1;

  char piece = '\0';
};
