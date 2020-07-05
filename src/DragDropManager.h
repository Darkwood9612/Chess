#pragma once
#include <string>
#include <cstdint>
#include <functional>

class DragDropManager
{
public:
  void SetDragItem(std::string name, int8_t x, int8_t y, char piece);
  void DropItem();

  std::string GetDragItemName() { return dragItemName; };
  int8_t GetDragItemStartPosX() { return dragItemStartPosX; };
  int8_t GetDragItemStartPosY() { return dragItemStartPosY; };
  int8_t GetDragItemCurrPosX() { return dragItemCurrPosX; };
  int8_t GetDragItemCurrPosY() { return dragItemCurrPosY; };
  char GetDragPiece() { return piece; };

  void SetDragItemCurrPosX(int8_t x) { dragItemCurrPosX = x; };
  void SetDragItemCurrPosY(int8_t y) { dragItemCurrPosY = y; };

  void SetDragCallback(std::function<void(int8_t, int8_t)> fn);
  void SetDropCallback(std::function<void(int8_t, int8_t)> fn);

  bool IsEmpty() { return dragItemName == " "; };

private:
  std::function<void(int8_t, int8_t)> StartDragCallback;
  std::function<void(int8_t, int8_t)> StartDropCallback;

  std::string dragItemName = " ";
  int8_t dragItemStartPosY = -1;
  int8_t dragItemStartPosX = -1;
  int8_t dragItemCurrPosY = -1;
  int8_t dragItemCurrPosX = -1;
  char piece = '\0';
};