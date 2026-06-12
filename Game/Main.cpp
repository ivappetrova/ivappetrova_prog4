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
#include "Commands/PickUpCommand.h"
#include "Commands/MoveCommand.h"
#include "Commands/JumpCommand.h"
#include "Commands/ShootCommand.h"

#include "Components/PlayerComponent.h"
#include "Components/PhysicsComponent.h"
#include "Components/BoxColliderComponent.h"
#include "Components/BoxColliderDebugDrawComponent.h"

#include "LevelLoader.h"
#include "Components/LevelManagerComponent.h"

#include <filesystem>
#include <memory>
#include <vector>
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

static dae::GameObject* MakePlayer(dae::Scene& scene, const std::string& texture,
	float startX, float startY, float windowHeight)
{
	auto characterGO = std::make_unique<dae::GameObject>();
	characterGO->AddComponent<dae::TextureComponent>(60.f, 60.f)->SetTexture(texture);
	characterGO->SetLocalPosition(startX, startY);

	characterGO->AddComponent<dae::HealthComponent>(3);
	characterGO->AddComponent<dae::ScoreComponent>();
	characterGO->AddComponent<dae::PhysicsComponent>(windowHeight);
	characterGO->AddComponent<dae::PlayerComponent>(150.f, scene);
	characterGO->AddComponent<dae::BoxColliderComponent>(60.f, 60.f);
	characterGO->AddComponent<dae::BoxColliderDebugDrawComponent>();

	dae::GameObject* pChar = characterGO.get();
	pChar->m_RenderOrder = 10;
	scene.Add(std::move(characterGO));

	return pChar;
}

static void MakeHUD(dae::Scene& scene, dae::GameObject* pPlayer, float labelX, float labelY, std::shared_ptr<dae::Font> font)
{
	auto* pHealth = pPlayer->GetComponent<dae::HealthComponent>();
	auto* pPoints = pPlayer->GetComponent<dae::ScoreComponent>();

	// Lives
	auto livesGO = std::make_unique<dae::GameObject>();
	livesGO->SetLocalPosition(labelX, labelY);
	auto* pLivesDisplay = livesGO->AddComponent<dae::HealthDisplayComponent>(4);
	scene.Add(std::move(livesGO));
	if (pHealth) pHealth->AddObserver(pLivesDisplay);

	// Points
	auto pointsGO = std::make_unique<dae::GameObject>();
	pointsGO->SetLocalPosition(labelX, labelY + 30.f);
	pointsGO->AddComponent<dae::TextComponent>("Points: 0", font);
	auto* pPointsDisplay = pointsGO->AddComponent<dae::ScoreDisplayComponent>();
	scene.Add(std::move(pointsGO));
	if (pPoints) pPoints->AddObserver(pPointsDisplay);
}

static void load(float windowWidth, float windowHeight)
{
	auto& scene = dae::SceneManager::GetInstance().CreateScene();

	auto font20 = dae::ResourceManager::GetInstance().LoadFont("Fonts/pixelify.ttf", 36);

	auto& soundSystem = dae::ServiceLocator::GetSoundSystem();
	const dae::sound_id SND_HIT = soundSystem.AddSound("Data/Sounds/sound1.mp3");
	const dae::sound_id SND_DEATH = soundSystem.AddSound("Data/Sounds/sound2.mp3");
	const dae::sound_id SND_POINT = soundSystem.AddSound("Data/Sounds/sound3.mp3");

	dae::GameObject* pChar1 = MakePlayer(scene, "bubble.png", 100.f, 100.f, windowHeight);
	MakeHUD(scene, pChar1, 100.f, 100.f, font20);

	auto& input = dae::InputManager::GetInstance();

	input.BindKeyboardCommand(SDL_SCANCODE_1, dae::InputManager::KeyState::Up, std::make_unique<dae::PlaySoundCommand>(SND_HIT, 0.8f));
	input.BindKeyboardCommand(SDL_SCANCODE_2, dae::InputManager::KeyState::Up, std::make_unique<dae::PlaySoundCommand>(SND_DEATH, 1.0f));
	input.BindKeyboardCommand(SDL_SCANCODE_3, dae::InputManager::KeyState::Up, std::make_unique<dae::PlaySoundCommand>(SND_POINT, 0.6f));

	input.BindKeyboardCommand(SDL_SCANCODE_A, dae::InputManager::KeyState::Down, std::make_unique<dae::MoveCommand>(pChar1, -1.f));
	input.BindKeyboardCommand(SDL_SCANCODE_A, dae::InputManager::KeyState::Up, std::make_unique<dae::MoveCommand>(pChar1, 0.f));
	input.BindKeyboardCommand(SDL_SCANCODE_D, dae::InputManager::KeyState::Down, std::make_unique<dae::MoveCommand>(pChar1, 1.f));
	input.BindKeyboardCommand(SDL_SCANCODE_D, dae::InputManager::KeyState::Up, std::make_unique<dae::MoveCommand>(pChar1, 0.f));
	input.BindKeyboardCommand(SDL_SCANCODE_W, dae::InputManager::KeyState::Down, std::make_unique<dae::JumpCommand>(pChar1));
	input.BindKeyboardCommand(SDL_SCANCODE_K, dae::InputManager::KeyState::Down, std::make_unique<dae::ShootCommand>(pChar1, scene));

	auto pLoader = std::make_shared<dae::LevelLoader>("Data/enemies.json");

	auto managerGO = std::make_unique<dae::GameObject>();
	managerGO->AddComponent<dae::LevelManagerComponent>(
		scene,
		pLoader,
		std::vector<dae::GameObject*>{ pChar1 },
		windowWidth,
		windowHeight);
	scene.Add(std::move(managerGO));
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

#if _DEBUG && defined(_WIN32) && !defined(__EMSCRIPTEN__)
	SpawnConsole();
#endif

	engine.Run([&]()
		{
			load(static_cast<float>(engine.GetWindowWidth()),
				static_cast<float>(engine.GetWindowHeight()));
		});

	dae::ServiceLocator::RegisterSoundSystem(nullptr);
	return 0;
}