#include "MainAppDelegate.h"

#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl2.h>
#include <SDL.h>

namespace Chess
{
	void AppDelegate::Init(const int, char** args)
	{
		if (IsInitialized()) {
			DeInit();
		}

		isWindowCreated_ = CreateSDLWindow();

		if (!isWindowCreated_) {
			throw std::exception("Error in AppDelegate::Init() -> CreateSDLWindow() == false");
		}

		gameContext_ = std::make_unique<GameContext>(std::filesystem::path(args[0]).parent_path());
		gameContext_->Init();
	}

	bool AppDelegate::CreateSDLWindow()
	{
		// Setup SDL
		if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0) {
			printf("Fatal initialization SDL error:: %s\n", SDL_GetError());
			return false;
		}

		// Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
		// GL ES 2.0 + GLSL 100
		const char* glsl_version = "#version 100";
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#elif defined(__APPLE__)
		// GL 3.2 Core + GLSL 150
		const char* glsl_version = "#version 150";
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS,
							SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#else
		// GL 3.0 + GLSL 130
		const char* glsl_version = "#version 130";
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#endif

		// From 2.0.18: Enable native IME.
#ifdef SDL_HINT_IME_SHOW_UI
		SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
#endif

		// Create window with graphics context
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
		constexpr auto windowFlags = static_cast<SDL_WindowFlags>(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);

		SDL_Window* window = SDL_CreateWindow(WINDOW_TITLE.data(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
											  GameContext::PIECE_IMAGE_SIZE * GameContext::NUMBER_OF_FIELD_COLUMNS,
											  GameContext::PIECE_IMAGE_SIZE * GameContext::NUMBER_OF_FIELD_COLUMNS, windowFlags);

		if (window == nullptr) {
			printf("Error: SDL_CreateWindow(): %s\n", SDL_GetError());
			return false;
		}

		const SDL_GLContext gl_context = SDL_GL_CreateContext(window);
		SDL_GL_MakeCurrent(window, gl_context);
		SDL_GL_SetSwapInterval(1); // Enable vsync

		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		ImGuiIO& io = ImGui::GetIO();

		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		// ImGui::StyleColorsLight();

		// Setup Platform/Renderer backends
		ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
		ImGui_ImplOpenGL3_Init(glsl_version);

		windowAndContext_ = std::make_pair(window, gl_context);

		return true;
	}

	void AppDelegate::DestroySDLWindow()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplSDL2_Shutdown();
		ImGui::DestroyContext();

		SDL_GL_DeleteContext(windowAndContext_.second);
		SDL_DestroyWindow(windowAndContext_.first);
		SDL_Quit();
	}

	void AppDelegate::DeInit()
	{
		if (!IsInitialized()) {
			return;
		}

		gameContext_->DeInit();
		gameContext_.reset();

		DestroySDLWindow();
	}

	void AppDelegate::Update(const float dt)
	{
		if (!IsInitialized()) {
			return;
		}

		SDL_Event event;

		while (SDL_PollEvent(&event)) {
			ImGui_ImplSDL2_ProcessEvent(&event);

			if (event.type == SDL_QUIT) {
				needEndGame_ = true;
			}

			if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE
				&& event.window.windowID == SDL_GetWindowID(windowAndContext_.first)) {
				needEndGame_ = true;
			}
		}

		gameContext_->Update(dt);
	}

	void AppDelegate::Draw() const
	{
		if (!IsInitialized()) {
			return;
		}

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();

		gameContext_->Draw();

		const ImGuiIO& io = ImGui::GetIO();
		constexpr auto clearColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

		ImGui::Render();

		glViewport(0, 0, static_cast<GLsizei>(io.DisplaySize.x), static_cast<GLsizei>(io.DisplaySize.y));
		glClearColor(clearColor.x * clearColor.w, clearColor.y * clearColor.w, clearColor.z * clearColor.w, clearColor.w);
		glClear(GL_COLOR_BUFFER_BIT);

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		SDL_GL_SwapWindow(windowAndContext_.first);
	}

	bool AppDelegate::IsInitialized() const noexcept
	{
		return gameContext_ != nullptr && isWindowCreated_;
	}

	bool AppDelegate::NeedToEndGame() const noexcept
	{
		return needEndGame_;
	}

} // namespace Chess
