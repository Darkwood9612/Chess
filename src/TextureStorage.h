#pragma once
#include <SDL_opengl.h>
#include <SDL_surface.h>
#include <map>
#include <string>

class TextureStorage
{
public:
  void ImageLoad(char id, const std::string& path);

  GLuint LookupTextureById(char id);

private:
  GLuint LoadIMG_AndConvertToTexture(const std::string& name);
  GLuint SurfaceToTexture(SDL_Surface* surface);

  std::map<const char, GLuint> storage;
};