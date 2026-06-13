#include "PvPState.h"
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
#include "Components/PlayerComponent.h"
#include "Components/PhysicsComponent.h"
#include "Components/BoxColliderComponent.h"
#include "Components/BoxColliderDebugDrawComponent.h"
#include "Components/LevelCollisionComponent.h"
#include "Components/LevelDebugDrawComponent.h"

#include "Commands/MoveCommand.h"
#include "Commands/JumpCommand.h"
#include "Commands/MuteCommand.h"
#include "Commands/ShootCommand.h"
#include "MenuCommands/GoToMenuCommand.h"

#include "Utils/SoundObserver.h"

#include "EndScreen/ScoreScreenData.h"
#include "EndScreen/ScoreScreenState.h"

#include <SDL3/SDL.h>
#include <memory>
#include <vector>

namespace dae
{
	static GameObject* MakePlayerPvP(Scene& scene, const std::string& texture, float startX, float startY, float windowHeight)
	{
		auto playerGO = std::make_unique<GameObject>();

		const float PLAYER_SIZE{ 60.f };
		const float PLAYER_SPEED{ 150.f };

		playerGO->AddComponent<TextureComponent>(PLAYER_SIZE, PLAYER_SIZE)->SetTexture(texture);
		playerGO->SetLocalPosition(startX, startY);

		playerGO->AddComponent<HealthComponent>(4);
		playerGO->AddComponent<PhysicsComponent>(windowHeight);
		playerGO->AddComponent<PlayerComponent>(PLAYER_SPEED, scene);
		playerGO->AddComponent<BoxColliderComponent>(PLAYER_SIZE, PLAYER_SIZE);
		playerGO->AddComponent<BoxColliderDebugDrawComponent>();

		playerGO->m_RenderOrder = 10;

		GameObject* pPlayer = playerGO.get();
		scene.Add(std::move(playerGO));
		return pPlayer;
	}

	static void MakeHUDPvP(Scene& scene, GameObject* pPlayer, float labelX, float labelY, std::shared_ptr<Font> font)
	{
		auto* pHealth = pPlayer->GetComponent<HealthComponent>();

		auto livesGO = std::make_unique<GameObject>();
		livesGO->SetLocalPosition(labelX, labelY);
		livesGO->AddComponent<TextComponent>("Lives: 4", font);
		auto* pLivesDisplay = livesGO->AddComponent<HealthDisplayComponent>(pHealth);
		scene.Add(std::move(livesGO));
		if (pHealth) pHealth->AddObserver(pLivesDisplay);
	}

	PvPState::PvPState(GameStateManager& gsm, float w, float h)
		: m_GSM(gsm), m_WindowWidth(w), m_WindowHeight(h)
	{
	}

	void PvPState::Enter()
	{
		auto& scene = SceneManager::GetInstance().CreateScene();
		auto  font20 = ResourceManager::GetInstance().LoadFont("Fonts/pixelify.ttf", 36);

		auto& soundSystem = ServiceLocator::GetSoundSystem();
		const sound_id SOUND_SHOOT = soundSystem.AddSound("Data/Sounds/sound1.mp3");
		const sound_id SOUND_HIT = soundSystem.AddSound("Data/Sounds/sound2.mp3");

		// Players
		GameObject* pChar1 = MakePlayerPvP(scene, "Characters/Player/Bubble.png", 100.f, 100.f, m_WindowHeight);
		GameObject* pChar2 = MakePlayerPvP(scene, "Characters/Maita/Maita.png", m_WindowWidth - 160.f, 100.f, m_WindowHeight);

		m_pChar1 = pChar1;
		m_pChar2 = pChar2;
		m_GameOver = false;

		// Level
		auto levelGO = std::make_unique<GameObject>();
		levelGO->SetLocalPosition(0.f, m_WindowHeight);
		levelGO->AddComponent<TextureComponent>(m_WindowWidth, m_WindowHeight)->SetTexture("Maps/level3.png");
		auto* pLevelCol = levelGO->AddComponent<LevelCollisionComponent>();
		pLevelCol->LoadFromSVG("Data/Maps/level3.svg", m_WindowWidth, m_WindowHeight);
		//levelGO->AddComponent<LevelDebugDrawComponent>(pLevelCol);
		scene.Add(std::move(levelGO));

		for (auto* pPlayer : { pChar1, pChar2 })
		{
			if (auto* pPhysics = pPlayer->GetComponent<PhysicsComponent>())
			{
				pPhysics->SetLevelCollision(pLevelCol);
			}
		}
			

		// HUDs
		MakeHUDPvP(scene, pChar1, 100.f, 100.f, font20);
		MakeHUDPvP(scene, pChar2, m_WindowWidth - 250.f, 100.f, font20);

		// Hit sounds — stored as members, no raw new
		m_HitObserver1.emplace(SOUND_HIT, EVENT_PLAYER_HIT);
		m_HitObserver2.emplace(SOUND_HIT, EVENT_PLAYER_HIT);

		if (auto* pHP = pChar1->GetComponent<HealthComponent>())
		{
			pHP->AddObserver(&m_HitObserver1.value());
		}
		if (auto* pHP = pChar2->GetComponent<HealthComponent>())
		{
			pHP->AddObserver(&m_HitObserver2.value());
		}

		auto& input = InputManager::GetInstance();

		// Player 1 : keyboard
		input.BindKeyboardCommand(SDL_SCANCODE_A, InputManager::KeyState::Pressed, std::make_unique<MoveCommand>(pChar1, -1.f));
		input.BindKeyboardCommand(SDL_SCANCODE_D, InputManager::KeyState::Pressed, std::make_unique<MoveCommand>(pChar1, +1.f));
		input.BindKeyboardCommand(SDL_SCANCODE_W, InputManager::KeyState::Down, std::make_unique<JumpCommand>(pChar1));
		input.BindKeyboardCommand(SDL_SCANCODE_P, InputManager::KeyState::Down, std::make_unique<ShootCommand>(pChar1, scene, SOUND_SHOOT, true));
		input.BindKeyboardCommand(SDL_SCANCODE_F2, InputManager::KeyState::Down, std::make_unique<MuteCommand>());
		input.BindKeyboardCommand(SDL_SCANCODE_ESCAPE, InputManager::KeyState::Down, std::make_unique<GoToMenuCommand>(m_GSM, m_WindowWidth, m_WindowHeight));

		// Player 1 : controller 1
		input.BindControllerCommand(1, Controller::Button::DPadLeft, Controller::KeyState::Pressed, std::make_unique<MoveCommand>(pChar1, -1.f));
		input.BindControllerCommand(1, Controller::Button::DPadRight, Controller::KeyState::Pressed, std::make_unique<MoveCommand>(pChar1, +1.f));
		input.BindControllerCommand(1, Controller::Button::ButtonA, Controller::KeyState::Down, std::make_unique<JumpCommand>(pChar1));
		input.BindControllerCommand(1, Controller::Button::ButtonB, Controller::KeyState::Down, std::make_unique<ShootCommand>(pChar1, scene, SOUND_SHOOT, true));
		input.BindControllerCommand(1, Controller::Button::LeftShoulder, Controller::KeyState::Down, std::make_unique<MuteCommand>());
		input.BindControllerCommand(1, Controller::Button::Back, Controller::KeyState::Down, std::make_unique<GoToMenuCommand>(m_GSM, m_WindowWidth, m_WindowHeight));

		// Player 2 : controller 0
		input.BindControllerCommand(0, Controller::Button::DPadLeft, Controller::KeyState::Pressed, std::make_unique<MoveCommand>(pChar2, -1.f));
		input.BindControllerCommand(0, Controller::Button::DPadRight, Controller::KeyState::Pressed, std::make_unique<MoveCommand>(pChar2, +1.f));
		input.BindControllerCommand(0, Controller::Button::ButtonA, Controller::KeyState::Down, std::make_unique<JumpCommand>(pChar2));
		input.BindControllerCommand(0, Controller::Button::ButtonB, Controller::KeyState::Down, std::make_unique<ShootCommand>(pChar2, scene, SOUND_SHOOT, true));
		input.BindControllerCommand(0, Controller::Button::LeftShoulder, Controller::KeyState::Down, std::make_unique<MuteCommand>());
		input.BindControllerCommand(0, Controller::Button::Back, Controller::KeyState::Down, std::make_unique<GoToMenuCommand>(m_GSM, m_WindowWidth, m_WindowHeight));

		// Music
		m_MusicId = soundSystem.AddSound("Data/Sounds/music.mp3");
		soundSystem.PlayLoop(m_MusicId, 0.05f);
	}

	void PvPState::Exit()
	{
		ServiceLocator::GetSoundSystem().Stop(m_MusicId);
		InputManager::GetInstance().UnbindAll();
		SceneManager::GetInstance().RemoveActiveScene();
	}

	void PvPState::Update(float /*deltaTime*/)
	{
		if (m_GameOver) return;

		auto isDead = [](GameObject* p)
			{
				if (!p) return true;
				auto* pH = p->GetComponent<HealthComponent>();
				return !pH || pH->GetHP() <= 0;
			};

		const bool P1_DEAD = isDead(m_pChar1);
		const bool P2_DEAD = isDead(m_pChar2);

		if (P1_DEAD || P2_DEAD)
		{
			m_GameOver = true;
			ScoreScreenData data;
			data.mode = GameMode::PvP;
			data.winnerIndex = P1_DEAD ? 1 : 0;
			m_GSM.SwitchTo(std::make_unique<ScoreScreenState>(
				m_GSM, m_WindowWidth, m_WindowHeight, std::move(data)));
		}
	}

	void PvPState::Render() const {}
}