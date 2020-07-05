#pragma once
#include "ChessDefines.h"
#include "GameModel.h"
#include <SDL_opengl.h>
#include <SDL_surface.h>
#include <map>
#include <string>
#include <vector>

class TextureStorage
{
public:
  TextureStorage() { fieldBackgroung = -1; };
  ~TextureStorage(){};

  void LoadPiecesImage(
    const std::vector<std::pair<EPiece, std::string>>& piecesImagePath);
  void LoadFieldBackround(const std::string& path);

  GLuint GetFieldBackroundTextureId() { return fieldBackgroung; };
  GLuint GetPieceTextureId(Game::Piece piece);

private:
  GLuint LoadIMG_AndConvertToTexture(const std::string& name);
  GLuint SurfaceToTexture(SDL_Surface* surface);

  GLuint fieldBackgroung;
  std::map<const EPiece, GLuint> pieceStorage;
};