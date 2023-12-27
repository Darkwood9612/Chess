#include "MainAppDelegate.h"

#if defined(IMGUI_IMPL_OPENGL_ES2)
	#include <SDL_opengles2.h>
#else
	#include <SDL_opengl.h>
#endif

#include <filesystem>
#include <thread>

int main(int countArgs, char** args)
{
	try {
		Chess::AppDelegate appDelegate {};

		appDelegate.Init(countArgs, args);

		auto lastTime = std::chrono::high_resolution_clock::now();

#ifdef __EMSCRIPTEN__
		// For an Emscripten build we are disabling file-system access, so let's not
		// attempt to do a fopen() of the imgui.ini file. You may manually call
		// LoadIniSettingsFromMemory() to load settings from your own storage.
		const ImGuiIO& io = ImGui::GetIO();
		io.IniFilename = nullptr;

		EMSCRIPTEN_MAINLOOP_BEGIN
#else
		while (!appDelegate.NeedToEndGame())
#endif
		{
			auto currentTime = std::chrono::high_resolution_clock::now();
			const float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
			lastTime = currentTime;

			appDelegate.Update(deltaTime);

			appDelegate.Draw();

			std::this_thread::sleep_for(std::chrono::milliseconds(1)); // To prevent CPU from maxing out
		}
#ifdef __EMSCRIPTEN__
		EMSCRIPTEN_MAINLOOP_END;
#endif

		appDelegate.DeInit();

		return 0;
	}
	catch (const std::exception& e) {
		MessageBox(nullptr, e.what(), nullptr, MB_OK | MB_ICONERROR);
	}

	return 1;
};