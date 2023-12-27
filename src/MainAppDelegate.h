#pragma once
#include "GameContext.h"

#include <memory>

namespace Chess
{
	class AppDelegate final
	{
	public:
		AppDelegate() = default;
		~AppDelegate() = default;

		AppDelegate(const AppDelegate&) = delete;
		AppDelegate& operator=(const AppDelegate&) = delete;

		AppDelegate(AppDelegate&&) = delete;
		AppDelegate& operator=(AppDelegate&&) = delete;

	public:
		void Init(int countArgs, char** args);
		void DeInit();

		void Update(float dt);

		void Draw() const;

		[[nodiscard]] bool IsInitialized() const noexcept;
		[[nodiscard]] bool NeedToEndGame() const noexcept;

	private:
		[[nodiscard]] bool CreateSDLWindow();

		void DestroySDLWindow();

	private:
		bool isWindowCreated_ { false };
		bool needEndGame_ { false };

		std::unique_ptr<GameContext> gameContext_ { nullptr };
		std::pair<SDL_Window*, SDL_GLContext> windowAndContext_ { nullptr, nullptr };

		static inline constexpr std::string_view WINDOW_TITLE = "Chess";
	};
} // namespace Chess
