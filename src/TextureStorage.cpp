#include "TextureStorage.h"

#include "ChessDefines.h"

#include <SDL_image.h>
#include <stdexcept>
#include <vector>

namespace Chess
{

	void TextureStorage::Init(const std::filesystem::path& absolutePath)
	{
		const std::vector<std::pair<EPiece, std::filesystem::path>> piecesImage = {
			{ EPiece::W_PAWN, absolutePath / "image/white_pawn_large.png" },   { EPiece::W_ROOK, absolutePath / "image/white_rook_large.png" },
			{ EPiece::W_QUEEN, absolutePath / "image/white_queen_large.png" }, { EPiece::W_KNIGHT, absolutePath / "image/white_knight_large.png" },
			{ EPiece::W_KING, absolutePath / "image/white_king_large.png" },   { EPiece::W_BISHOP, absolutePath / "image/white_bishop_large.png" },

			{ EPiece::B_PAWN, absolutePath / "image/black_pawn_large.png" },   { EPiece::B_ROOK, absolutePath / "image/black_rook_large.png" },
			{ EPiece::B_QUEEN, absolutePath / "image/black_queen_large.png" }, { EPiece::B_KNIGHT, absolutePath / "image/black_knight_large.png" },
			{ EPiece::B_KING, absolutePath / "image/black_king_large.png" },   { EPiece::B_BISHOP, absolutePath / "image/black_bishop_large.png" }
		};

		for (auto& [pieceType, path] : piecesImage) {
			ImageLoad(std::to_string(static_cast<char>(pieceType)), path.string());
		}

		ImageLoad(std::string(BACKGROUND_ID), (absolutePath / "image/green_board_large.png").string());
	}

	void TextureStorage::ImageLoad(const std::string& id, const std::string& path)
	{
		_storage[id] = LoadIMG_AndConvertToTexture(path);
	}

	GLuint TextureStorage::LookupTextureById(const std::string& id) const

	{
		const auto it = _storage.find(id);

		return it == _storage.end() ? -1 : it->second;
	}

	GLuint TextureStorage::LoadIMG_AndConvertToTexture(const std::string& name)
	{
		SDL_Surface* surface = IMG_Load(name.data());

		if (!surface) {
			std::string err = name + " == nullptr" + std::string(SDL_GetError());
			throw std::runtime_error(err.data());
		}

		const GLuint result = SurfaceToTexture(surface);

		SDL_FreeSurface(surface);

		return result;
	}

	GLuint TextureStorage::SurfaceToTexture(const SDL_Surface* surface)
	{
		GLuint texture = 0;

		if (!surface) {
			throw std::runtime_error("surface == nullptr");
		}

		int mode;

		switch (surface->format->BytesPerPixel) {
			case 4:
				if (surface->format->Rmask == 0x000000ff)
					mode = GL_RGBA;
				else
					mode = GL_BGRA;
				break;

			case 3:
				if (surface->format->Rmask == 0x000000ff)
					mode = GL_RGB;
				else
					mode = GL_BGR;
				break;

			default:
				throw std::runtime_error("Error, image is not true color");
		}

		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);

		glTexImage2D(GL_TEXTURE_2D, 0, mode, surface->w, surface->h, 0, mode, GL_UNSIGNED_BYTE, surface->pixels);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		return texture;
	}
} // namespace Chess