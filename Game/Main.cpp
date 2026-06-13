#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#if _DEBUG && __has_include(<vld.h>)
#include <vld.h>
#endif

#include "Minigin.h"
#include "ServiceLocator.h"
#include "SoundSystem.h"
#include "LoggingSoundSystem.h"

#include "GameStateManager.h"
#include "MenuState.h"

#include <filesystem>
#include <memory>

namespace fs = std::filesystem;

#if defined(_WIN32) && !defined(__EMSCRIPTEN__) && _DEBUG
#include <windows.h>
static void SpawnConsole()
{
	AllocConsole();
	FILE* f;
	freopen_s(&f, "CONOUT$", "w", stdout);
	freopen_s(&f, "CONOUT$", "w", stderr);
	freopen_s(&f, "CONIN$", "r", stdin);
}
#endif

int main(int, char* [])
{
#if __EMSCRIPTEN__
	fs::path data_location = "";
#else
	fs::path data_location = "./Data/";
	if (!fs::exists(data_location))
		data_location = "../Data/";
#endif

	dae::Minigin engine(data_location);

#if _DEBUG
	dae::ServiceLocator::RegisterSoundSystem( std::make_unique<dae::LoggingSoundSystem>( std::make_unique<dae::SoundSystem>()));
#else
	dae::ServiceLocator::RegisterSoundSystem(
		std::make_unique<dae::SoundSystem>());
#endif

#if _DEBUG && defined(_WIN32) && !defined(__EMSCRIPTEN__)
	SpawnConsole();
#endif

	// The GameStateManager owns the current state and drives transitions.
	// It lives for the entire lifetime of engine.Run().
	auto gsm = std::make_unique<dae::GameStateManager>();

	engine.Run([&]()
		{
			const float w = static_cast<float>(engine.GetWindowWidth());
			const float h = static_cast<float>(engine.GetWindowHeight());
			engine.SetGameStateManager(gsm.get());  // hook into loop
			gsm->SwitchTo(std::make_unique<dae::MenuState>(*gsm, w, h));
			gsm->FlushPendingTransition();  // flush the initial state immediately
		});

	dae::ServiceLocator::RegisterSoundSystem(nullptr);
	return 0;
}