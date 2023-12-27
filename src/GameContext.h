#pragma once
#include "ChessEngine.h"
#include "DragDropManager.h"
#include "Move.h"
#include "TextureStorage.h"

#include <filesystem>
#include <SDL_video.h>

namespace Chess
{
	class GameContext final
	{
	public:
		explicit GameContext(const std::filesystem::path& absolutePath);

		void Init();
		void DeInit();

		void Update(float dt);

		void Draw();

	private:
		void DrawOfferToPlayWithAI();
		void DrawWrongMoveErrors();
		void DrawDraggingCell();
		void DrawBoard();

		void InitDragAndDrop();

		void SayAiMakeMove();

		using StartGameFunction = std::function<void(bool playerWontPlayWithAi)>;

		StartGameFunction GetStartGameAndChoseRival();

	public:
		static inline constexpr uint32_t NUMBER_OF_FIELD_COLUMNS = 8;
		static inline constexpr uint32_t PIECE_IMAGE_SIZE = 90;

	private:
		TextureStorage _textureStorage {};

		ChessEngine _engine;
		ChessEngine _engineForAi;

		Move _move {};
		TurnControl _turnControl {};
		DragDropManager _dragDropManager {};

		std::filesystem::path _absolutePath {};

		std::list<std::string> _uiNotifications {};

		static inline constexpr std::string_view ENGINE_PATH = "engine/stockfish_20011801_x64.exe";
	};
} // namespace Chess
