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

#include "Components/HealthComponent.h"
#include "Components/HealthDisplayComponent.h"
#include "Components/PlayerDiedDisplayComponent.h"
#include "Components/ScoreComponent.h"
#include "Components/ScoreDisplayComponent.h"

#include "ServiceLocator.h"
#include "SoundSystem.h"
#include "LoggingSoundSystem.h"

#include "Commands/PlaySoundCommand.h"
#include "Commands/DealDamageCommand.h"
#include "Commands/PickUpCommand.h"
#include "Commands/MoveCommand.h"
#include "Commands/JumpCommand.h"

#include "Components/PlayerComponent.h"
#include "Components/PhysicsComponent.h"

#include "Components/LevelCollisionComponent.h"
#include "Components/LevelDebugDrawComponent.h"
#include "Components/BoxColliderComponent.h"
#include "Components/BoxColliderDebugDrawComponent.h"

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

static dae::GameObject* MakePlayer(dae::Scene& scene, const std::string& texture, float startX, float startY, float labelX, std::shared_ptr<dae::Font> font, float windowHeight)
{
	auto characterGO = std::make_unique<dae::GameObject>();
	characterGO->AddComponent<dae::TextureComponent>(60.f, 60.f)->SetTexture(texture);
	characterGO->SetLocalPosition(startX, startY);

	auto* pHealth = characterGO->AddComponent<dae::HealthComponent>(3);
	auto* pPoints = characterGO->AddComponent<dae::ScoreComponent>();\
	const float PLAYER_SPEED{ 150.f };
	characterGO->AddComponent<dae::PhysicsComponent>(windowHeight);
	characterGO->AddComponent<dae::PlayerComponent>(PLAYER_SPEED);

	characterGO->AddComponent<dae::BoxColliderComponent>(60.f, 60.f);
	characterGO->AddComponent<dae::BoxColliderDebugDrawComponent>();

	dae::GameObject* pChar = characterGO.get();
	scene.Add(std::move(characterGO));

	auto livesGO = std::make_unique<dae::GameObject>();
	livesGO->SetLocalPosition(labelX, 100);
	livesGO->AddComponent<dae::TextComponent>("Lives: 3", font);
	auto* pLivesDisplay = livesGO->AddComponent<dae::HealthDisplayComponent>(3);
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
	auto* pPointsDisplay = pointsGO->AddComponent<dae::ScoreDisplayComponent>();
	scene.Add(std::move(pointsGO));
	pPoints->AddObserver(pPointsDisplay);

	return pChar;
}

static void load(float windowWidth, float windowHeight)
{
	auto& scene = dae::SceneManager::GetInstance().CreateScene();

	// Level

	auto bg = std::make_unique<dae::GameObject>();
	bg->SetLocalPosition(0.f, windowHeight);
	bg->AddComponent<dae::TextureComponent>(windowWidth, windowHeight)->SetTexture("background.png");
	scene.Add(std::move(bg));

	auto levelGO = std::make_unique<dae::GameObject>();
	levelGO->SetLocalPosition(0.f, windowHeight);
	levelGO->AddComponent<dae::TextureComponent>(windowWidth, windowHeight)->SetTexture("Maps/level3.png");
	auto* pLevelCol = levelGO->AddComponent<dae::LevelCollisionComponent>();
	pLevelCol->LoadFromSVG("Data/Maps/level3.svg", windowWidth, windowHeight);

	levelGO->AddComponent<dae::LevelDebugDrawComponent>(pLevelCol);
	scene.Add(std::move(levelGO));

	//

	auto font36 = dae::ResourceManager::GetInstance().LoadFont("Fonts/Lingua.otf", 36);
	auto font20 = dae::ResourceManager::GetInstance().LoadFont("Fonts/Lingua.otf", 20);

	auto fpsGo = std::make_unique<dae::GameObject>();
	fpsGo->SetLocalPosition(10, 10);
	fpsGo->AddComponent<dae::TextComponent>("FPS: 0", font36);
	fpsGo->AddComponent<dae::FPSComponent>();
	scene.Add(std::move(fpsGo));

	auto& soundSystem = dae::ServiceLocator::GetSoundSystem();
	const dae::sound_id SND_HIT = soundSystem.AddSound("Sounds/sound1.mp3");
	const dae::sound_id SND_DEATH = soundSystem.AddSound("Sounds/sound2.mp3");
	const dae::sound_id SND_POINT = soundSystem.AddSound("Sounds/sound3.mp3");

	auto& input = dae::InputManager::GetInstance();

	input.BindKeyboardCommand(SDL_SCANCODE_1, dae::InputManager::KeyState::Up, std::make_unique<dae::PlaySoundCommand>(SND_HIT, 0.8f));
	input.BindKeyboardCommand(SDL_SCANCODE_2, dae::InputManager::KeyState::Up, std::make_unique<dae::PlaySoundCommand>(SND_DEATH, 1.0f));
	input.BindKeyboardCommand(SDL_SCANCODE_3, dae::InputManager::KeyState::Up, std::make_unique<dae::PlaySoundCommand>(SND_POINT, 0.6f));

	const int DAMAGE{ 1 };
	const int POINTS{ 20 };

	////////////// Bubble
	dae::GameObject* pChar1 = MakePlayer(scene, "bubble.png", 100.f, 100.f, 10.f, font20, windowHeight);
	pChar1->GetComponent<dae::PhysicsComponent>()->SetLevelCollision(pLevelCol);

	auto bubbleLabel = std::make_unique<dae::GameObject>();
	bubbleLabel->SetLocalPosition(100, 60);
	bubbleLabel->AddComponent<dae::TextComponent>("Bubble", font20);
	scene.Add(std::move(bubbleLabel));

	auto bubbleMove = std::make_unique<dae::GameObject>();
	bubbleMove->SetLocalPosition(100, 160);
	bubbleMove->AddComponent<dae::TextComponent>("Move: A/D", font20);
	scene.Add(std::move(bubbleMove));

	auto bubbleDmg = std::make_unique<dae::GameObject>();
	bubbleDmg->SetLocalPosition(100, 190);
	bubbleDmg->AddComponent<dae::TextComponent>("Deal Dmg + Sound: O", font20);
	scene.Add(std::move(bubbleDmg));

	auto bubblePickup = std::make_unique<dae::GameObject>();
	bubblePickup->SetLocalPosition(100, 220);
	bubblePickup->AddComponent<dae::TextComponent>("Pick Up + Sound: P", font20);
	scene.Add(std::move(bubblePickup));

	// Bubble move — Pressed sets direction, Up clears it so Idle is re-entered on release
	input.BindKeyboardCommand(SDL_SCANCODE_A, dae::InputManager::KeyState::Pressed, std::make_unique<dae::MoveCommand>(pChar1, -1.f));
	input.BindKeyboardCommand(SDL_SCANCODE_A, dae::InputManager::KeyState::Up, std::make_unique<dae::MoveCommand>(pChar1, 0.f));
	input.BindKeyboardCommand(SDL_SCANCODE_W, dae::InputManager::KeyState::Down, std::make_unique<dae::JumpCommand>(pChar1));
	input.BindKeyboardCommand(SDL_SCANCODE_D, dae::InputManager::KeyState::Pressed, std::make_unique<dae::MoveCommand>(pChar1, 1.f));
	input.BindKeyboardCommand(SDL_SCANCODE_D, dae::InputManager::KeyState::Up, std::make_unique<dae::MoveCommand>(pChar1, 0.f));

	///////// Bobble
	dae::GameObject* pChar2 = MakePlayer(scene, "bobble.png", 500.f, 100.f, 500.f, font20, windowHeight);
	pChar2->GetComponent<dae::PhysicsComponent>()->SetLevelCollision(pLevelCol);

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

	// Bobble move — Pressed sets direction, Up clears it so Idle is re-entered on release
	input.BindControllerCommand(0, dae::Controller::Button::DPadLeft, dae::Controller::KeyState::Pressed, std::make_unique<dae::MoveCommand>(pChar2, -1.f));
	input.BindControllerCommand(0, dae::Controller::Button::DPadLeft, dae::Controller::KeyState::Up, std::make_unique<dae::MoveCommand>(pChar2, 0.f));
	input.BindControllerCommand(0, dae::Controller::Button::DPadRight, dae::Controller::KeyState::Pressed, std::make_unique<dae::MoveCommand>(pChar2, 1.f));
	input.BindControllerCommand(0, dae::Controller::Button::DPadRight, dae::Controller::KeyState::Up, std::make_unique<dae::MoveCommand>(pChar2, 0.f));

	input.BindControllerCommand(0, dae::Controller::Button::ButtonX, dae::Controller::KeyState::Up, std::make_unique<dae::DealDamageCommand>(pChar1, DAMAGE, pChar2));
	input.BindControllerCommand(0, dae::Controller::Button::ButtonX, dae::Controller::KeyState::Up, std::make_unique<dae::PlaySoundCommand>(SND_HIT, 0.8f));
	input.BindControllerCommand(0, dae::Controller::Button::ButtonA, dae::Controller::KeyState::Up, std::make_unique<dae::PickUpCommand>(pChar2, POINTS));
	input.BindControllerCommand(0, dae::Controller::Button::ButtonA, dae::Controller::KeyState::Up, std::make_unique<dae::PlaySoundCommand>(SND_POINT, 0.6f));

	// Bub input
	input.BindKeyboardCommand(SDL_SCANCODE_O, dae::InputManager::KeyState::Up, std::make_unique<dae::DealDamageCommand>(pChar2, DAMAGE, pChar1));
	input.BindKeyboardCommand(SDL_SCANCODE_O, dae::InputManager::KeyState::Up, std::make_unique<dae::PlaySoundCommand>(SND_HIT, 0.8f));
	input.BindKeyboardCommand(SDL_SCANCODE_P, dae::InputManager::KeyState::Up, std::make_unique<dae::PickUpCommand>(pChar1, POINTS));
	input.BindKeyboardCommand(SDL_SCANCODE_P, dae::InputManager::KeyState::Up, std::make_unique<dae::PlaySoundCommand>(SND_POINT, 0.6f));

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

	engine.Run([&]() { load(static_cast<float>(engine.GetWindowWidth()), static_cast<float>(engine.GetWindowHeight())); });

	dae::ServiceLocator::RegisterSoundSystem(nullptr);

	return 0;
}