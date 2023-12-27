#include "GameContext.h"

#include "CommandGenerator.h"
#include "Utils.h"

#include <imgui.h>

namespace
{
	void AddImageToPieceBackgroundDrawList(const Chess::EPiece pieceType, const ImVec2 piecePos, const Chess::TextureStorage& textureStorage)
	{
		if (IsEmpty(pieceType)) {
			return;
		}

		const auto textureId = textureStorage.LookupTextureById(std::to_string(static_cast<char>(pieceType)));

		const auto texturePtr = reinterpret_cast<ImTextureID>(textureId);

		ImGui::GetBackgroundDrawList()->AddImage(texturePtr, ImVec2(piecePos.x, piecePos.y),
												 ImVec2(Chess::GameContext::PIECE_IMAGE_SIZE + piecePos.x, Chess::GameContext::PIECE_IMAGE_SIZE + piecePos.y));
	}
} // namespace

namespace Chess
{
	GameContext::GameContext(const std::filesystem::path& absolutePath)
		: _engine((absolutePath / ENGINE_PATH).string())
		, _engineForAi((absolutePath / ENGINE_PATH).string())
		, _absolutePath(absolutePath)
	{ }

	void GameContext::Init()
	{
		_textureStorage.Init(_absolutePath);
		InitDragAndDrop();
	}

	void GameContext::DeInit() { }

	void GameContext::Update(float)
	{
		SayAiMakeMove();

		if (!ImGui::IsMouseDragging(0) && _dragDropManager.HasDraggedPiece()) {
			_dragDropManager.DropDraggedPiece();
		}

		if (_turnControl.IsGameOver() && _uiNotifications.empty()) {
			_turnControl.StartNewGame();
		}
	}

	void GameContext::Draw()
	{
		if (!_turnControl.IsPlayerMadeChoice()) {
			DrawOfferToPlayWithAI();
			return;
		}

		DrawBoard();
		DrawDraggingCell();
		DrawWrongMoveErrors();
	}

	void GameContext::DrawOfferToPlayWithAI()
	{
		const ImGuiIO& io = ImGui::GetIO();

		ImGui::SetNextWindowSize(ImVec2(400, 200), ImGuiCond_Once);

		ImGui::Begin("OfferToPlayWithAI", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

		ImGui::SetWindowPos(ImVec2(io.DisplaySize.x / 4, io.DisplaySize.y / 2 - ImGui::GetWindowHeight()), ImGuiCond_Once);

		constexpr ImVec2 buttonSize(50, 30);

		ImGui::NewLine();
		ImGui::Text("\t\t\tYou wont play with AI?");
		ImGui::NewLine();
		ImGui::Indent(ImGui::GetWindowWidth() / 2 - (buttonSize.x * 2));

		if (ImGui::Button("Yes", buttonSize)) {
			GetStartGameAndChoseRival()(true);
		}

		ImGui::SameLine();

		if (ImGui::Button("No", buttonSize)) {
			GetStartGameAndChoseRival()(false);
		}

		ImGui::End();
	}

	void GameContext::DrawWrongMoveErrors()
	{
		if (_uiNotifications.empty()) {
			return;
		}

		const ImGuiIO& io = ImGui::GetIO();

		ImGui::SetNextWindowSize(ImVec2(400, 200), ImGuiCond_Once);

		ImGui::Begin("WrongMoveErrors", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

		ImGui::SetWindowPos(ImVec2(io.DisplaySize.x / 4, io.DisplaySize.y / 2 - ImGui::GetWindowHeight()), ImGuiCond_Once);

		constexpr ImVec2 buttonOkSize(50, 30);

		ImGui::NewLine();

		const auto text = _uiNotifications.front();

		ImGui::Text(text.c_str());

		ImGui::NewLine();

		ImGui::Indent(ImGui::GetWindowWidth() / 2 - buttonOkSize.x);

		if (ImGui::Button("Close", buttonOkSize)) {
			_uiNotifications.pop_front();
		}

		ImGui::End();
	}

	void GameContext::DrawDraggingCell()
	{
		if (!_dragDropManager.HasDraggedPiece()) {
			return;
		}

		const ImGuiIO& io = ImGui::GetIO();

		const float CUBE_HEIGHT = io.DisplaySize.y / NUMBER_OF_FIELD_COLUMNS;

		ImGui::SetNextWindowSize(ImVec2(PIECE_IMAGE_SIZE, PIECE_IMAGE_SIZE), ImGuiCond_Once);

		ImGui::SetNextWindowBgAlpha(0.0f);

		ImGui::SetNextWindowPos(ImVec2(static_cast<float>(_dragDropManager.GetDragItemStartPosX()) * CUBE_HEIGHT,
									   static_cast<float>(_dragDropManager.GetDragItemStartPosY()) * CUBE_HEIGHT),
								ImGuiCond_Once);

		constexpr ImGuiWindowFlags pieceFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollWithMouse;

		const auto currentDraggedCellNumber = std::to_string(_dragDropManager.GetCurrentDraggedCellNumber());

		ImGui::Begin(currentDraggedCellNumber.c_str(), nullptr, pieceFlags);

		const auto piecePos = ImGui::GetWindowPos();

		ImGui::GetStyle().WindowBorderSize = 1.f;

		const auto halfSquare = CUBE_HEIGHT / 2;

		const auto overlappedCellPosX = static_cast<int32_t>((piecePos.x + halfSquare) / CUBE_HEIGHT);
		const auto overlappedCellPosY = static_cast<int32_t>((piecePos.y + halfSquare) / CUBE_HEIGHT);
		const auto overlappedCellPos = static_cast<int32_t>(overlappedCellPosX + overlappedCellPosY * NUMBER_OF_FIELD_COLUMNS);

		_dragDropManager.SetCurrentOverlappedCellId(overlappedCellPos);

		AddImageToPieceBackgroundDrawList(_dragDropManager.GetCurrentDraggedPieceType(), piecePos, _textureStorage);

		ImGui::End();
	}

	void GameContext::DrawBoard()
	{
		const ImGuiIO& io = ImGui::GetIO();

		const float CUBE_HEIGHT = io.DisplaySize.y / NUMBER_OF_FIELD_COLUMNS;

		ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Once);
		ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x, io.DisplaySize.y), ImGuiCond_Once);
		ImGui::SetNextWindowBgAlpha(0.f);

		ImGui::Begin("Game", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollWithMouse);

		ImGui::GetBackgroundDrawList()->AddImage((void*)_textureStorage.LookupTextureById(std::string(TextureStorage::BACKGROUND_ID)), ImVec2(0, 0),
												 ImVec2(io.DisplaySize.x, io.DisplaySize.y));

		const auto field = utils::FennToArray(_engine.GetFen());

		for (size_t x = 0; x < NUMBER_OF_FIELD_COLUMNS; ++x) {
			for (size_t y = 0; y < NUMBER_OF_FIELD_COLUMNS; ++y) {
				const auto cellNumber = static_cast<int32_t>(x + y * NUMBER_OF_FIELD_COLUMNS);
				const auto piece = field[cellNumber];

				/// if the piece is dragged, it will be drawn last
				if (_dragDropManager.HasDraggedPiece() && _dragDropManager.GetCurrentDraggedCellNumber() == cellNumber) {
					continue;
				}

				/// Draw Board Piece
				ImGui::SetNextWindowSize(ImVec2(PIECE_IMAGE_SIZE, PIECE_IMAGE_SIZE), ImGuiCond_Once);
				ImGui::SetNextWindowBgAlpha(0.0f);
				ImGui::SetNextWindowPos(ImVec2(static_cast<float>(x) * CUBE_HEIGHT, static_cast<float>(y) * CUBE_HEIGHT));

				ImGuiWindowFlags pieceFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollWithMouse;

				const bool isMyTurnToMove = _engine.IsWhiteTurnToMove() != IsBlack(piece) && _engine.IsWhiteTurnToMove() == IsWhite(piece);

				const bool pieceCannotBeMoved = !isMyTurnToMove || !_turnControl.IsPlayerMadeChoice() || (_turnControl.IsPlayerPlayWithAI() && IsBlack(piece));

				if (pieceCannotBeMoved) {
					pieceFlags |= ImGuiWindowFlags_NoMove;
				}

				ImGui::Begin(std::to_string(cellNumber).c_str(), nullptr, pieceFlags);

				const auto piecePos = ImGui::GetWindowPos();
				ImGui::GetStyle().WindowBorderSize = 0.f;

				if (ImGui::BeginDragDropSource()) {
					if (!pieceCannotBeMoved) {
						const auto halfSquare = CUBE_HEIGHT / 2;

						const float overlappedCellPosX = (piecePos.x + halfSquare) / CUBE_HEIGHT;
						const float overlappedCellPosY = (piecePos.y + halfSquare) / CUBE_HEIGHT;

						_dragDropManager.SetDragItem(cellNumber, static_cast<int32_t>(overlappedCellPosX), static_cast<int32_t>(overlappedCellPosY), piece);
					}

					ImGui::EndDragDropSource();
				}

				AddImageToPieceBackgroundDrawList(piece, piecePos, _textureStorage);

				ImGui::End();
			}
		}

		ImGui::End();
	}

	void GameContext::SayAiMakeMove()
	{
		/// Say AI make a move if he thought enough
		if (_turnControl.IsPlayerMadeChoice() && _turnControl.IsPlayerPlayWithAI() && !_engine.IsWhiteTurnToMove() && _turnControl.IsEngineThoughtEnough()) {
			// Stop thinking
			_engineForAi.SendCommand(CommandGenerator::Stop());

			std::ignore = _engineForAi.IsReady();

			std::vector<std::string> answers;

			auto lastAnswer = _engineForAi.GetLastAnswer();

			utils::split(lastAnswer, answers, " ");

			for (size_t i = 0; i < answers.size(); ++i) {
				if (answers[i].find("bestmove") != std::string::npos) {
					_move.SetMove(answers[i + 1]);

					_dragDropManager.DropDraggedPiece(true);
				}
			}
		}
	}

	GameContext::StartGameFunction GameContext::GetStartGameAndChoseRival()
	{
		return [this](const bool playerWontPlayWithAi) {
			_engine.SendCommand(CommandGenerator::NewGame());

			Sleep(300);
			_engine.UpdateFen();

			_engineForAi.SendCommand(CommandGenerator::NewGame());

			Sleep(300);
			_engineForAi.UpdateFen();

			_turnControl.SetPlayerMadeChoice(playerWontPlayWithAi);
		};
	}

	void GameContext::InitDragAndDrop()
	{
		_dragDropManager.SetDragCallback([this](const int32_t dragCellId) {
			_move.SetFrom(dragCellId, NUMBER_OF_FIELD_COLUMNS);
		});

		_dragDropManager.SetDropCallback([this](const bool isAi, const int32_t dropCellId) {
			if (!isAi) {
				_move.SetTo(dropCellId, NUMBER_OF_FIELD_COLUMNS);
			}

			const auto fen = _engine.GetFen();

			_engine.SendCommand(CommandGenerator::Position(fen, _move.GetMove()));
			_engine.UpdateFen();

			const bool isMoveWrong = _engine.GetFen() == fen && !_move.IsFromEqualTo();

			if (isMoveWrong) {
				_uiNotifications.emplace_back("\tError: move " + _move.GetMove() + " is considered unacceptable!");
			}

			switch (_engine.IsSomebodyWon()) {
				case WinningSide::Black:
				{
					_uiNotifications.emplace_back("\tBlack Player Win. GameOver!");
					_turnControl.SetGameOver(WinningSide::Black);
				} break;
				case WinningSide::White:
				{
					_uiNotifications.emplace_back("\tWhite Player Win. GameOver!");
					_turnControl.SetGameOver(WinningSide::White);

				} break;
				case WinningSide::NoOne:
				{
					if (_engine.IsWhiteTurnToMove()) {
						break;
					}

					_engineForAi.SendCommand(CommandGenerator::Position(_engine.GetFen()));

					_engineForAi.SendCommand(CommandGenerator::GoSearchInfinite());
					_turnControl.SetStartThinkTime();

				} break;
			}
		});
	}
} // namespace Chess
