#include "CoopState.h"
#include "GameStateManager.h"

#include "SceneManager.h"
#include "ResourceManager.h"
#include "InputManager.h"
#include "ServiceLocator.h"
#include "SoundSystem.h"
#include "Scene.h"
#include "GameObject.h"

#include "TextureComponent.h"
#include "TextComponent.h"

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
#include "MenuCommands/GoToMenuCommand.h"

#include "Utils/SoundObserver.h"
#include "Utils/LevelLoader.h"

#include "EndScreen/ScoreScreenData.h"
#include "EndScreen/ScoreScreenState.h"

#include <SDL3/SDL.h>
#include <memory>
#include <vector>

namespace dae
{
	static GameObject* MakePlayerMP(Scene& scene, const std::string& texture, float startX, float startY, float windowHeight)
	{
		auto playerGO = std::make_unique<GameObject>();

		const float PLAYER_SIZE{ 60.f };
		const float PLAYER_SPEED{ 150.f };

		playerGO->AddComponent<TextureComponent>(PLAYER_SIZE, PLAYER_SIZE)->SetTexture(texture);
		playerGO->SetLocalPosition(startX, startY);

		playerGO->AddComponent<HealthComponent>(4);
		playerGO->AddComponent<ScoreComponent>();
		playerGO->AddComponent<PhysicsComponent>(windowHeight);
		playerGO->AddComponent<PlayerComponent>(PLAYER_SPEED, scene);
		playerGO->AddComponent<BoxColliderComponent>(PLAYER_SIZE, PLAYER_SIZE);
		playerGO->AddComponent<BoxColliderDebugDrawComponent>();

		playerGO->m_RenderOrder = 10;

		GameObject* pPlayer = playerGO.get();
		scene.Add(std::move(playerGO));
		return pPlayer;
	}

	static void MakeHUDMP(Scene& scene, GameObject* pPlayer, float labelX, float labelY, std::shared_ptr<Font> font)
	{
		auto* pHealth = pPlayer->GetComponent<HealthComponent>();
		auto* pPoints = pPlayer->GetComponent<ScoreComponent>();

		auto livesGO = std::make_unique<GameObject>();
		livesGO->SetLocalPosition(labelX, labelY);
		livesGO->AddComponent<TextComponent>("Lives: 4", font);
		auto* pLivesDisplay = livesGO->AddComponent<HealthDisplayComponent>(pHealth);
		scene.Add(std::move(livesGO));
		if (pHealth) pHealth->AddObserver(pLivesDisplay);

		auto pointsGO = std::make_unique<GameObject>();
		pointsGO->SetLocalPosition(labelX, labelY + 30.f);
		pointsGO->AddComponent<TextComponent>("Points: 0", font);
		auto* pPointsDisplay = pointsGO->AddComponent<ScoreDisplayComponent>();
		scene.Add(std::move(pointsGO));
		if (pPoints) pPoints->AddObserver(pPointsDisplay);
	}

	CoopState::CoopState(GameStateManager& gsm, float w, float h)
		: m_GSM(gsm), m_WindowWidth(w), m_WindowHeight(h)
	{
	}

	void CoopState::Enter()
	{
		auto& scene = SceneManager::GetInstance().CreateScene();
		auto  font20 = ResourceManager::GetInstance().LoadFont("Fonts/pixelify.ttf", 36);

		// Sounds
		auto& soundSystem = ServiceLocator::GetSoundSystem();
		const sound_id SOUND_SHOOT = soundSystem.AddSound("Data/Sounds/sound1.mp3");
		const sound_id SOUND_HIT = soundSystem.AddSound("Data/Sounds/sound2.mp3");
		const sound_id SOUND_POINT = soundSystem.AddSound("Data/Sounds/sound3.mp3");

		// Players
		GameObject* pChar1 = MakePlayerMP(scene, "Characters/Player/Bubble.png", 100.f, 100.f, m_WindowHeight);
		GameObject* pChar2 = MakePlayerMP(scene, "Characters/Player/Bobble.png", m_WindowWidth - 160.f, 100.f, m_WindowHeight);

		// HUDs
		MakeHUDMP(scene, pChar1, 100.f, 100.f, font20);
		MakeHUDMP(scene, pChar2, m_WindowWidth - 250.f, 100.f, font20);

		// Sound observers — both players share the same sounds
		m_HitObserver1.emplace(SOUND_HIT, EVENT_PLAYER_HIT);
		m_HitObserver2.emplace(SOUND_HIT, EVENT_PLAYER_HIT);
		m_PointObserver1.emplace(SOUND_POINT, EVENT_PLAYER_GET_POINTS);
		m_PointObserver2.emplace(SOUND_POINT, EVENT_PLAYER_GET_POINTS);

		if (auto* pHP = pChar1->GetComponent<HealthComponent>())
		{
			pHP->AddObserver(&m_HitObserver1.value());
		}
		if (auto* pHP = pChar2->GetComponent<HealthComponent>())
		{
			pHP->AddObserver(&m_HitObserver2.value());
		}
		if (auto* pScore = pChar1->GetComponent<ScoreComponent>())
		{
			pScore->AddObserver(&m_PointObserver1.value());
		}
		if (auto* pScore = pChar2->GetComponent<ScoreComponent>())
		{
			pScore->AddObserver(&m_PointObserver2.value());
		}

		auto pLoader = std::make_shared<LevelLoader>("Data/enemies.json");
		auto managerGO = std::make_unique<GameObject>();
		managerGO->AddComponent<LevelManagerComponent>( scene, pLoader, std::vector<GameObject*>{ pChar1, pChar2 }, m_WindowWidth, m_WindowHeight);
		GameObject* pManagerRaw = managerGO.get();

		pManagerRaw->GetComponent<LevelManagerComponent>()->SetOnGameOver([&, pChar1, pChar2]()
			{
				ScoreScreenData data;
				data.mode = GameMode::Coop;
				if (auto* pScore = pChar1->GetComponent<ScoreComponent>())
					data.playerScores.push_back(pScore->GetScore());
				if (auto* pScore = pChar2->GetComponent<ScoreComponent>())
					data.playerScores.push_back(pScore->GetScore());
				m_GSM.SwitchTo(std::make_unique<ScoreScreenState>(m_GSM, m_WindowWidth, m_WindowHeight, std::move(data)));
			});

		scene.Add(std::move(managerGO));

		auto& input = InputManager::GetInstance();

		// Player 1 : keyboard 
		input.BindKeyboardCommand(SDL_SCANCODE_A, InputManager::KeyState::Pressed, std::make_unique<MoveCommand>(pChar1, -1.f));
		input.BindKeyboardCommand(SDL_SCANCODE_D, InputManager::KeyState::Pressed, std::make_unique<MoveCommand>(pChar1, +1.f));
		input.BindKeyboardCommand(SDL_SCANCODE_W, InputManager::KeyState::Down, std::make_unique<JumpCommand>(pChar1));
		input.BindKeyboardCommand(SDL_SCANCODE_P, InputManager::KeyState::Down, std::make_unique<ShootCommand>(pChar1, scene, SOUND_SHOOT));
		input.BindKeyboardCommand(SDL_SCANCODE_F1, InputManager::KeyState::Up, std::make_unique<SkipLevelCommand>(pManagerRaw));
		input.BindKeyboardCommand(SDL_SCANCODE_F2, InputManager::KeyState::Down, std::make_unique<MuteCommand>());
		input.BindKeyboardCommand(SDL_SCANCODE_ESCAPE, InputManager::KeyState::Down, std::make_unique<GoToMenuCommand>(m_GSM, m_WindowWidth, m_WindowHeight));

		// or controller 1
		input.BindControllerCommand(1, Controller::Button::DPadLeft, Controller::KeyState::Pressed, std::make_unique<MoveCommand>(pChar1, -1.f));
		input.BindControllerCommand(1, Controller::Button::DPadRight, Controller::KeyState::Pressed, std::make_unique<MoveCommand>(pChar1, +1.f));
		input.BindControllerCommand(1, Controller::Button::ButtonA, Controller::KeyState::Down, std::make_unique<JumpCommand>(pChar1));
		input.BindControllerCommand(1, Controller::Button::ButtonB, Controller::KeyState::Down, std::make_unique<ShootCommand>(pChar1, scene, SOUND_SHOOT));
		input.BindControllerCommand(1, Controller::Button::LeftShoulder, Controller::KeyState::Down, std::make_unique<MuteCommand>());
		input.BindControllerCommand(1, Controller::Button::RightShoulder, Controller::KeyState::Down, std::make_unique<SkipLevelCommand>(pManagerRaw));
		input.BindControllerCommand(1, Controller::Button::Back, Controller::KeyState::Down, std::make_unique<GoToMenuCommand>(m_GSM, m_WindowWidth, m_WindowHeight));

		// Player 2 : controller 0
		input.BindControllerCommand(0, Controller::Button::DPadLeft, Controller::KeyState::Pressed, std::make_unique<MoveCommand>(pChar2, -1.f));
		input.BindControllerCommand(0, Controller::Button::DPadRight, Controller::KeyState::Pressed, std::make_unique<MoveCommand>(pChar2, +1.f));
		input.BindControllerCommand(0, Controller::Button::ButtonA, Controller::KeyState::Down, std::make_unique<JumpCommand>(pChar2));
		input.BindControllerCommand(0, Controller::Button::ButtonB, Controller::KeyState::Down, std::make_unique<ShootCommand>(pChar2, scene, SOUND_SHOOT));
		input.BindControllerCommand(0, Controller::Button::LeftShoulder, Controller::KeyState::Down, std::make_unique<MuteCommand>());
		input.BindControllerCommand(0, Controller::Button::RightShoulder, Controller::KeyState::Down, std::make_unique<SkipLevelCommand>(pManagerRaw));
		input.BindControllerCommand(0, Controller::Button::Back, Controller::KeyState::Down, std::make_unique<GoToMenuCommand>(m_GSM, m_WindowWidth, m_WindowHeight));


		// Background music
		m_MusicId = soundSystem.AddSound("Data/Sounds/music.mp3");
		soundSystem.PlayLoop(m_MusicId, 0.05f);
	}

	void CoopState::Exit()
	{
		ServiceLocator::GetSoundSystem().Stop(m_MusicId);

		InputManager::GetInstance().UnbindAll();
		SceneManager::GetInstance().RemoveActiveScene();
	}

	void CoopState::Update(float /*deltaTime*/)
	{
		// SceneManager::Update is driven by Minigin's loop.
	}

	void CoopState::Render() const
	{
		// SceneManager::Render is driven by Minigin's loop.
	}
}