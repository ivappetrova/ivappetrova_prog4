#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#if _DEBUG && __has_include(<vld.h>)
#include <vld.h>
#endif

#include "Minigin.h"
#include "SceneManager.h"
#include "ResourceManager.h"
#include "Scene.h"
#include "TextureComponent.h"
#include "TextComponent.h"
#include "FPSComponent.h"
#include "InputManager.h"
#include "HealthComponent.h"
#include "LivesDisplayComponent.h"
#include "PlayerDiedDisplayComponent.h"
#include "PlayerPointsComponent.h"
#include "PointsDisplayComponent.h"
#include "ServiceLocator.h"
#include "PlaySoundCommand.h"
#include "SoundSystem.h"
#include "LoggingSoundSystem.h"
#include "DealDamageCommand.h"
#include "PickUpCommand.h"
#include "MoveCommand.h"
#include "Player.h"

#include <filesystem>
namespace fs = std::filesystem;

// for debugging, might remove later
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

static dae::GameObject* MakePlayer(
	dae::Scene& scene,
	const std::string& texture,
	float startX, float startY,
	float labelX,
	std::shared_ptr<dae::Font> font)
{
	auto characterGO = std::make_unique<dae::GameObject>();
	characterGO->AddComponent<dae::TextureComponent>()->SetTexture(texture);
	characterGO->SetLocalPosition(startX, startY);

	auto* pHealth = characterGO->AddComponent<dae::HealthComponent>(3);
	auto* pPoints = characterGO->AddComponent<dae::PlayerPointsComponent>();
	characterGO->AddComponent<dae::Player>();

	dae::GameObject* pChar = characterGO.get();
	scene.Add(std::move(characterGO));

	auto livesGO = std::make_unique<dae::GameObject>();
	livesGO->SetLocalPosition(labelX, 100);
	livesGO->AddComponent<dae::TextComponent>("Lives: 3", font);
	auto* pLivesDisplay = livesGO->AddComponent<dae::LivesDisplayComponent>(3);
	scene.Add(std::move(livesGO));
	pHealth->AddObserver(pLivesDisplay);

	auto diedGO = std::make_unique<dae::GameObject>();
	diedGO->SetLocalPosition(labelX, 130);
	diedGO->AddComponent<dae::TextComponent>("", font);
	auto* pDiedDisplay = diedGO->AddComponent<dae::PlayerDiedDisplayComponent>();
	scene.Add(std::move(diedGO));
	pHealth->AddObserver(pDiedDisplay);

	auto pointsGO = std::make_unique<dae::GameObject>();
	pointsGO->SetLocalPosition(labelX, 250);
	pointsGO->AddComponent<dae::TextComponent>("Points: 0", font);
	auto* pPointsDisplay = pointsGO->AddComponent<dae::PointsDisplayComponent>();
	scene.Add(std::move(pointsGO));
	pPoints->AddObserver(pPointsDisplay);

	return pChar;
}

static void load()
{
	auto& scene = dae::SceneManager::GetInstance().CreateScene();

	auto bg = std::make_unique<dae::GameObject>();
	bg->AddComponent<dae::TextureComponent>()->SetTexture("background.png");
	scene.Add(std::move(bg));

	auto font36 = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 36);
	auto font20 = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 20);

	auto fpsGo = std::make_unique<dae::GameObject>();
	fpsGo->SetLocalPosition(10, 10);
	fpsGo->AddComponent<dae::TextComponent>("FPS: 0", font36);
	fpsGo->AddComponent<dae::FPSComponent>();
	scene.Add(std::move(fpsGo));

	auto& soundSystem = dae::ServiceLocator::GetSoundSystem();
	const dae::sound_id SND_HIT = soundSystem.AddSound("Data/sound1.mp3");
	const dae::sound_id SND_DEATH = soundSystem.AddSound("Data/sound2.mp3");
	const dae::sound_id SND_POINT = soundSystem.AddSound("Data/sound3.mp3");

	auto& input = dae::InputManager::GetInstance();

	input.BindKeyboardCommand(SDL_SCANCODE_1, dae::InputManager::KeyState::Up,
		std::make_unique<dae::PlaySoundCommand>(SND_HIT, 0.8f));
	input.BindKeyboardCommand(SDL_SCANCODE_2, dae::InputManager::KeyState::Up,
		std::make_unique<dae::PlaySoundCommand>(SND_DEATH, 1.0f));
	input.BindKeyboardCommand(SDL_SCANCODE_3, dae::InputManager::KeyState::Up,
		std::make_unique<dae::PlaySoundCommand>(SND_POINT, 0.6f));

	const int DAMAGE = 1;
	const int POINTS = 20;

	////////////// Bubble
	dae::GameObject* pChar1 = MakePlayer(scene, "bubble.png",
		10.f, 300.f, 10.f, font20);

	auto bubbleLabel = std::make_unique<dae::GameObject>();
	bubbleLabel->SetLocalPosition(10, 60);
	bubbleLabel->AddComponent<dae::TextComponent>("Bubble", font20);
	scene.Add(std::move(bubbleLabel));

	auto bubbleMove = std::make_unique<dae::GameObject>();
	bubbleMove->SetLocalPosition(10, 160);
	bubbleMove->AddComponent<dae::TextComponent>("Move: A/D", font20);
	scene.Add(std::move(bubbleMove));

	auto bubbleDmg = std::make_unique<dae::GameObject>();
	bubbleDmg->SetLocalPosition(10, 190);
	bubbleDmg->AddComponent<dae::TextComponent>("Deal Dmg + Sound: O", font20);
	scene.Add(std::move(bubbleDmg));

	auto bubblePickup = std::make_unique<dae::GameObject>();
	bubblePickup->SetLocalPosition(10, 220);
	bubblePickup->AddComponent<dae::TextComponent>("Pick Up + Sound: P", font20);
	scene.Add(std::move(bubblePickup));

	input.BindKeyboardCommand(SDL_SCANCODE_A, dae::InputManager::KeyState::Pressed,
		std::make_unique<dae::MoveCommand>(pChar1, -1.f));
	input.BindKeyboardCommand(SDL_SCANCODE_D, dae::InputManager::KeyState::Pressed,
		std::make_unique<dae::MoveCommand>(pChar1, 1.f));

	///////// Bobble
	dae::GameObject* pChar2 = MakePlayer(scene, "bobble.png",
		500.f, 300.f, 500.f, font20);

	auto bobbleLabel = std::make_unique<dae::GameObject>();
	bobbleLabel->SetLocalPosition(500, 60);
	bobbleLabel->AddComponent<dae::TextComponent>("Bobble", font20);
	scene.Add(std::move(bobbleLabel));

	auto bobbleMove = std::make_unique<dae::GameObject>();
	bobbleMove->SetLocalPosition(500, 160);
	bobbleMove->AddComponent<dae::TextComponent>("Move: D-Pad Left/Right", font20);
	scene.Add(std::move(bobbleMove));

	auto bobbleDmg = std::make_unique<dae::GameObject>();
	bobbleDmg->SetLocalPosition(500, 190);
	bobbleDmg->AddComponent<dae::TextComponent>("Deal Dmg + Sound: X (gamepad)", font20);
	scene.Add(std::move(bobbleDmg));

	auto bobblePickup = std::make_unique<dae::GameObject>();
	bobblePickup->SetLocalPosition(500, 220);
	bobblePickup->AddComponent<dae::TextComponent>("Pick Up + Sound: A (gamepad)", font20);
	scene.Add(std::move(bobblePickup));

	input.BindControllerCommand(0, dae::Controller::Button::DPadLeft, dae::Controller::KeyState::Pressed,
		std::make_unique<dae::MoveCommand>(pChar2, -1.f));
	input.BindControllerCommand(0, dae::Controller::Button::DPadRight, dae::Controller::KeyState::Pressed,
		std::make_unique<dae::MoveCommand>(pChar2, 1.f));

	input.BindControllerCommand(0, dae::Controller::Button::ButtonX, dae::Controller::KeyState::Up,
		std::make_unique<dae::DealDamageCommand>(pChar1, DAMAGE, pChar2));
	input.BindControllerCommand(0, dae::Controller::Button::ButtonX, dae::Controller::KeyState::Up,
		std::make_unique<dae::PlaySoundCommand>(SND_HIT, 0.8f));
	input.BindControllerCommand(0, dae::Controller::Button::ButtonA, dae::Controller::KeyState::Up,
		std::make_unique<dae::PickUpCommand>(pChar2, POINTS));
	input.BindControllerCommand(0, dae::Controller::Button::ButtonA, dae::Controller::KeyState::Up,
		std::make_unique<dae::PlaySoundCommand>(SND_POINT, 0.6f));

	// Bub input
	input.BindKeyboardCommand(SDL_SCANCODE_O, dae::InputManager::KeyState::Up,
		std::make_unique<dae::DealDamageCommand>(pChar2, DAMAGE, pChar1));
	input.BindKeyboardCommand(SDL_SCANCODE_O, dae::InputManager::KeyState::Up,
		std::make_unique<dae::PlaySoundCommand>(SND_HIT, 0.8f));
	input.BindKeyboardCommand(SDL_SCANCODE_P, dae::InputManager::KeyState::Up,
		std::make_unique<dae::PickUpCommand>(pChar1, POINTS));
	input.BindKeyboardCommand(SDL_SCANCODE_P, dae::InputManager::KeyState::Up,
		std::make_unique<dae::PlaySoundCommand>(SND_POINT, 0.6f));

	auto soundHud = std::make_unique<dae::GameObject>();
	soundHud->SetLocalPosition(10, 500);
	soundHud->AddComponent<dae::TextComponent>("1/2/3 - test sounds", font20);
	scene.Add(std::move(soundHud));
}

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
	dae::ServiceLocator::RegisterSoundSystem(
		std::make_unique<dae::LoggingSoundSystem>(
			std::make_unique<dae::SoundSystem>()));
#else
	dae::ServiceLocator::RegisterSoundSystem(
		std::make_unique<dae::SoundSystem>());
#endif

	// for debugging, might remove later
#if _DEBUG && defined(_WIN32) && !defined(__EMSCRIPTEN__)
	SpawnConsole();
#endif

	engine.Run(load);
	return 0;
}