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
#include "InputManager.h"
#include "ServiceLocator.h"
#include "SoundSystem.h"
#include "LoggingSoundSystem.h"

#include "SoundObserver.h"
#include "LevelLoader.h"

#include "Components/HealthComponent.h"
#include "Components/HealthDisplayComponent.h"
#include "Components/ScoreComponent.h"
#include "Components/ScoreDisplayComponent.h"
#include "Components/PlayerComponent.h"
#include "Components/PhysicsComponent.h"
#include "Components/BoxColliderComponent.h"
#include "Components/BoxColliderDebugDrawComponent.h"
#include "Components/LevelManagerComponent.h"

#include "Commands/MoveCommand.h"
#include "Commands/JumpCommand.h"
#include "Commands/ShootCommand.h"
#include "Commands/SkipLevelCommand.h"
#include "Commands/MuteCommand.h"

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

static dae::GameObject* MakePlayer(dae::Scene& scene, const std::string& texture, float startX, float startY, float windowHeight)
{
	auto playerGameObj = std::make_unique<dae::GameObject>();

	const float PLAYER_SIZE{ 60.f };
	const float PLAYER_SPEED{ 150.f };

	playerGameObj->AddComponent<dae::TextureComponent>(PLAYER_SIZE, PLAYER_SIZE)->SetTexture(texture);
	playerGameObj->SetLocalPosition(startX, startY);

	playerGameObj->AddComponent<dae::HealthComponent>(4);
	playerGameObj->AddComponent<dae::ScoreComponent>();
	playerGameObj->AddComponent<dae::PhysicsComponent>(windowHeight);
	playerGameObj->AddComponent<dae::PlayerComponent>(PLAYER_SPEED, scene);
	playerGameObj->AddComponent<dae::BoxColliderComponent>(PLAYER_SIZE, PLAYER_SIZE);
	playerGameObj->AddComponent<dae::BoxColliderDebugDrawComponent>();

	dae::GameObject* pPlayer = playerGameObj.get();
	pPlayer->m_RenderOrder = 10;
	scene.Add(std::move(playerGameObj));
	return pPlayer;
}

static void MakeHUD(dae::Scene& scene, dae::GameObject* pPlayer, float labelX, float labelY, std::shared_ptr<dae::Font> font)
{
	auto* pHealth = pPlayer->GetComponent<dae::HealthComponent>();
	auto* pPoints = pPlayer->GetComponent<dae::ScoreComponent>();

	// Lives
	auto livesGO = std::make_unique<dae::GameObject>();
	livesGO->SetLocalPosition(labelX, labelY);
	livesGO->AddComponent<dae::TextComponent>("Lives: 4", font);
	auto* pLivesDisplay = livesGO->AddComponent<dae::HealthDisplayComponent>(pHealth);
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
	// Scene
	auto& scene = dae::SceneManager::GetInstance().CreateScene();
	auto font20 = dae::ResourceManager::GetInstance().LoadFont("Fonts/pixelify.ttf", 36);

	// Sounds
	auto& soundSystem = dae::ServiceLocator::GetSoundSystem();
	const dae::sound_id SOUND_SHOOT = soundSystem.AddSound("Data/Sounds/sound1.mp3");
	const dae::sound_id SOUND_HIT = soundSystem.AddSound("Data/Sounds/sound2.mp3");
	const dae::sound_id SOUND_POINT = soundSystem.AddSound("Data/Sounds/sound3.mp3");

	// Player
	dae::GameObject* pChar1 = MakePlayer(scene, "bubble.png", 100.f, 100.f, windowHeight);
	MakeHUD(scene, pChar1, 100.f, 100.f, font20);

	// Sound observers
	if (auto* pHealth = pChar1->GetComponent<dae::HealthComponent>())
	{
		pHealth->AddObserver(new dae::SoundObserver{ SOUND_HIT, dae::EVENT_PLAYER_HIT });
	}

	if (auto* pScore = pChar1->GetComponent<dae::ScoreComponent>())
	{
		pScore->AddObserver(new dae::SoundObserver{ SOUND_POINT, dae::EVENT_PLAYER_GET_POINTS });
	}

	// Level manager
	auto pLoader = std::make_shared<dae::LevelLoader>("Data/enemies.json");
	auto managerGO = std::make_unique<dae::GameObject>();
	managerGO->AddComponent<dae::LevelManagerComponent>(scene, pLoader, std::vector<dae::GameObject*>{ pChar1 }, windowWidth, windowHeight);
	auto* pManagerRaw = managerGO.get();
	scene.Add(std::move(managerGO));

	// Input
	auto& input = dae::InputManager::GetInstance();
	input.BindKeyboardCommand(SDL_SCANCODE_A, dae::InputManager::KeyState::Pressed, std::make_unique<dae::MoveCommand>(pChar1, -1.f));
	input.BindKeyboardCommand(SDL_SCANCODE_D, dae::InputManager::KeyState::Pressed, std::make_unique<dae::MoveCommand>(pChar1, 1.f));
	input.BindKeyboardCommand(SDL_SCANCODE_W, dae::InputManager::KeyState::Down, std::make_unique<dae::JumpCommand>(pChar1));
	input.BindKeyboardCommand(SDL_SCANCODE_P, dae::InputManager::KeyState::Down, std::make_unique<dae::ShootCommand>(pChar1, scene, SOUND_SHOOT));
	input.BindKeyboardCommand(SDL_SCANCODE_F1, dae::InputManager::KeyState::Up, std::make_unique<dae::SkipLevelCommand>(pManagerRaw));
	input.BindKeyboardCommand(SDL_SCANCODE_F2, dae::InputManager::KeyState::Down, std::make_unique<dae::MuteCommand>());

	// Background music
	const dae::sound_id SOUND_MUSIC = soundSystem.AddSound("Data/Sounds/music.mp3");
	soundSystem.PlayLoop(SOUND_MUSIC, 0.05f);
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