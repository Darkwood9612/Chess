#include "TextureStorage.h"
#include "SDL_image.h"
#include <stdexcept>

GLuint TextureStorage::LoadIMG_AndConvertToTexture(const std::string& name)
{
  SDL_Surface* surface = IMG_Load(name.data());
  GLuint result = -1;

  if (!surface) {
    std::string err = name + " == nullptr";
    throw std::runtime_error(err.data());
  }

  result = SurfaceToTexture(surface);
  SDL_FreeSurface(surface);

  return result;
}

GLuint TextureStorage::SurfaceToTexture(SDL_Surface* surface)
{
  unsigned int texture = NULL;
  if (texture != 0)
    glDeleteTextures(1, &texture);

  if (!surface)
    throw std::runtime_error("surface == nullptr");

  int Mode = GL_RGB;
  switch (surface->format->BytesPerPixel) {
    case 4:
      if (surface->format->Rmask == 0x000000ff)
        Mode = GL_RGBA;
      else
        Mode = GL_BGRA;
      break;

    case 3:
      if (surface->format->Rmask == 0x000000ff)
        Mode = GL_RGB;
      else
        Mode = GL_BGR;
      break;

    default:
      throw std::runtime_error("Error, image is not truecolor");
  }

  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);

  glTexImage2D(GL_TEXTURE_2D, 0, Mode, surface->w, surface->h, 0, Mode,
               GL_UNSIGNED_BYTE, surface->pixels);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  return texture;
}

void TextureStorage::LoadPiecesImage(
  const std::vector<std::pair<EPiece, std::string>>& piecesImagePath)
{
  for (auto& item : piecesImagePath) {
    pieceStorage[item.first] = LoadIMG_AndConvertToTexture(item.second);
  }
}

void TextureStorage::LoadFieldBackround(const std::string& path)
{
  fieldBackgroung = LoadIMG_AndConvertToTexture(path);
}

GLuint TextureStorage::GetPieceTextureId(Game::Piece piece)
{
  auto it = pieceStorage.find(piece.type);
  return it == pieceStorage.end() ? -1 : it->second;
}
