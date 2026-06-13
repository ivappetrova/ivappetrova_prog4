#include "MenuState.h"
#include "GameStateManager.h"
#include "GameModes/SinglePlayerState.h"
#include "GameModes/MultiplayerState.h"

#include "SceneManager.h"
#include "ResourceManager.h"
#include "InputManager.h"
#include "ServiceLocator.h"
#include "Scene.h"
#include "GameObject.h"
#include "TextureComponent.h"
#include "TextComponent.h"

#include "MenuCommands/MenuNavigateCommand.h"
#include "MenuCommands/MenuConfirmCommand.h"
#include "Commands/MuteCommand.h"

#include <SDL3/SDL.h>

namespace dae
{
	// Tweak these to position the menu text over your background image
	static constexpr float OPTION1_X = 380.f;
	static constexpr float OPTION1_Y = 580.f;
	static constexpr float OPTION2_Y = 630.f;
	static constexpr float SELECTOR_OFFSET_X = -40.f; // sits to the left of the text

	MenuState::MenuState(GameStateManager& gsm, float windowWidth, float windowHeight)
		: m_GSM(gsm)
		, m_WindowWidth(windowWidth)
		, m_WindowHeight(windowHeight)
	{}

	void MenuState::Enter()
	{
		m_Selected = 0;
		m_pSelector = nullptr;

		auto& scene = SceneManager::GetInstance().CreateScene();
		auto  font = ResourceManager::GetInstance().LoadFont("Fonts/pixelify.ttf", 36);
		auto  fontSmall = ResourceManager::GetInstance().LoadFont("Fonts/pixelify.ttf", 24);

		// Background
		auto bgGO = std::make_unique<GameObject>();
		bgGO->SetLocalPosition(140.f, 600.f);
		bgGO->AddComponent<TextureComponent>(800.f, 600.f) ->SetTexture("Menu/MainMenu.png"); 
		bgGO->m_RenderOrder = 0;
		scene.Add(std::move(bgGO));

		// Instructions1
		auto instructionsGO = std::make_unique<GameObject>();
		instructionsGO->SetLocalPosition(20.f, 30.f);
		instructionsGO->AddComponent<TextComponent>("Navigate: W/S or DPad Up/Down", fontSmall);
		instructionsGO->m_RenderOrder = 1;
		scene.Add(std::move(instructionsGO));
		// Instructions2
		auto instructionsGO2 = std::make_unique<GameObject>();
		instructionsGO2->SetLocalPosition(20.f, 60.f);
		instructionsGO2->AddComponent<TextComponent>("Confirm: Space or Button A", fontSmall);
		instructionsGO2->m_RenderOrder = 1;
		scene.Add(std::move(instructionsGO2));
		// Instructions3
		auto instructionsGO3 = std::make_unique<GameObject>();
		instructionsGO3->SetLocalPosition(20.f, 90.f);
		instructionsGO3->AddComponent<TextComponent>("Mute: F2 or Button Start", fontSmall);
		instructionsGO3->m_RenderOrder = 1;
		scene.Add(std::move(instructionsGO3));


		// Option1
		auto opt1GO = std::make_unique<GameObject>();
		opt1GO->SetLocalPosition(OPTION1_X, OPTION1_Y + 10.f);
		opt1GO->AddComponent<TextComponent>("Singleplayer Mode", font);
		opt1GO->m_RenderOrder = 1;
		scene.Add(std::move(opt1GO));

		// Option2
		auto opt2GO = std::make_unique<GameObject>();
		opt2GO->SetLocalPosition(OPTION1_X, OPTION2_Y + 10.f);
		opt2GO->AddComponent<TextComponent>("Multiplayer Mode", font);
		opt2GO->m_RenderOrder = 1;
		scene.Add(std::move(opt2GO));

		// Selector Sprite
		auto selectorGO = std::make_unique<GameObject>();
		m_pSelector = selectorGO.get();
		m_pSelector->AddComponent<TextureComponent>(20.f, 20.f) ->SetTexture("Menu/Selector.png");
		m_pSelector->m_RenderOrder = 2;
		UpdateSelectorPosition();
		scene.Add(std::move(selectorGO));

		// Music
		auto& sound = ServiceLocator::GetSoundSystem();
		m_MusicId = sound.AddSound("Data/Sounds/music.mp3");
		sound.PlayLoop(m_MusicId, 0.05f);

		// Input
		auto& input = InputManager::GetInstance();

		// Keyboard
		input.BindKeyboardCommand(SDL_SCANCODE_S, InputManager::KeyState::Down, std::make_unique<MenuNavigateCommand>(*this, +1));
		input.BindKeyboardCommand(SDL_SCANCODE_W, InputManager::KeyState::Down, std::make_unique<MenuNavigateCommand>(*this, -1));
		input.BindKeyboardCommand(SDL_SCANCODE_RETURN, InputManager::KeyState::Down, std::make_unique<MenuConfirmCommand>(*this));
		input.BindKeyboardCommand(SDL_SCANCODE_F2, InputManager::KeyState::Down, std::make_unique<MuteCommand>());

		// Controller
		input.BindControllerCommand(0, Controller::Button::DPadDown, Controller::KeyState::Down, std::make_unique<MenuNavigateCommand>(*this, +1));
		input.BindControllerCommand(0, Controller::Button::DPadUp, Controller::KeyState::Down, std::make_unique<MenuNavigateCommand>(*this, -1));
		input.BindControllerCommand(0, Controller::Button::ButtonA, Controller::KeyState::Down, std::make_unique<MenuConfirmCommand>(*this));
		input.BindControllerCommand(0, Controller::Button::LeftShoulder, Controller::KeyState::Down, std::make_unique<MuteCommand>());
	}

	void MenuState::Exit()
	{
		ServiceLocator::GetSoundSystem().Stop(m_MusicId);

		m_pSelector = nullptr;
		InputManager::GetInstance().UnbindAll();
		SceneManager::GetInstance().RemoveActiveScene();
	}

	void MenuState::Update(float /*deltaTime*/) {}
	void MenuState::Render() const {}

	void MenuState::Navigate(int delta)
	{
		m_Selected = (m_Selected + delta + NUM_OPTIONS) % NUM_OPTIONS;
		UpdateSelectorPosition();
	}

	void MenuState::Confirm()
	{
		switch (m_Selected)
		{
		case 0:
			m_GSM.SwitchTo(std::make_unique<SinglePlayerState>(m_GSM, m_WindowWidth, m_WindowHeight));
			break;
		case 1:
			m_GSM.SwitchTo(std::make_unique<MultiplayerState>(m_GSM, m_WindowWidth, m_WindowHeight));
			break;
		default:
			break;
		}
	}

	void MenuState::UpdateSelectorPosition()
	{
		if (!m_pSelector) return;
		const float y = (m_Selected == 0) ? OPTION1_Y : OPTION2_Y;
		// Centre the 30px sprite vertically on the text line
		m_pSelector->SetLocalPosition(OPTION1_X + SELECTOR_OFFSET_X, y + 3.f);
	}
}