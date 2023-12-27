#pragma once
#include <filesystem>
#include <map>
#include <SDL_opengl.h>
#include <SDL_surface.h>
#include <string>

namespace Chess
{
	class TextureStorage
	{
	public:
		void Init(const std::filesystem::path& absolutePath);

		void ImageLoad(const std::string& id, const std::string& path);

		[[nodiscard]] GLuint LookupTextureById(const std::string& id) const;

		static [[nodiscard]] GLuint SurfaceToTexture(const SDL_Surface* surface);
		static [[nodiscard]] GLuint LoadIMG_AndConvertToTexture(const std::string& name);

		static inline constexpr std::string_view BACKGROUND_ID = "Background";

	private:
		std::map<std::string, GLuint> _storage;
	};
}; // namespace Chess