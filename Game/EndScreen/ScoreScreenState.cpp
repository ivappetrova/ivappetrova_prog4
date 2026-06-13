#include "EndScreen/ScoreScreenState.h"
#include "GameStateManager.h"
#include "MenuState.h"

#include "SceneManager.h"
#include "ResourceManager.h"
#include "InputManager.h"
#include "ServiceLocator.h"
#include "Scene.h"
#include "GameObject.h"
#include "TextComponent.h"
#include "TextureComponent.h"

#include "ScoreManager.h"

#include <SDL3/SDL.h>
#include <sstream>

#include "Commands/MuteCommand.h"

namespace dae
{
	ScoreScreenState::ScoreScreenState(GameStateManager& gsm, float w, float h, ScoreScreenData data)
		: m_GSM(gsm), m_WindowWidth(w), m_WindowHeight(h), m_Data(std::move(data))
	{}

	void ScoreScreenState::Enter()
	{
		ScoreManager::GetInstance().Load();

		m_ActivePlayer = 0;
		m_CursorPos = 0;
		m_Confirmed = false;
		m_Letters = { 0, 0, 0 };
		m_InputTimer = 0.f;

		auto& scene = SceneManager::GetInstance().CreateScene();
		auto  pFont = ResourceManager::GetInstance().LoadFont("Fonts/pixelify.ttf", 36);
		auto  pFontSmall = ResourceManager::GetInstance().LoadFont("Fonts/pixelify.ttf", 24);

		const float CENTER_X = m_WindowWidth * 0.5f - 100.f;

		if (m_Data.mode == GameMode::PvP)
		{
			// ── PvP: just show the winner ────────────────────────────────
			const std::string WINNER_STR =
				(m_Data.winnerIndex == 0) ? "Player 1 Wins!" :
				(m_Data.winnerIndex == 1) ? "Player 2 Wins!" : "Draw!";

			auto pWinnerGO = std::make_unique<GameObject>();
			pWinnerGO->SetLocalPosition(CENTER_X - 60.f, 200.f);
			pWinnerGO->AddComponent<TextComponent>(WINNER_STR, pFont);
			pWinnerGO->m_RenderOrder = 1;
			scene.Add(std::move(pWinnerGO));

			auto pHintGO = std::make_unique<GameObject>();
			pHintGO->SetLocalPosition(CENTER_X - 80.f, 260.f);
			pHintGO->AddComponent<TextComponent>("Press Enter / A to return", pFontSmall);
			pHintGO->m_RenderOrder = 1;
			scene.Add(std::move(pHintGO));
		}
		else
		{
			// ── SP/Coop: name entry + leaderboard ───────────────────────
			int totalScore = 0;
			for (int s : m_Data.playerScores) totalScore += s;

			auto pScoreGO = std::make_unique<GameObject>();
			pScoreGO->SetLocalPosition(CENTER_X, 60.f);
			pScoreGO->AddComponent<TextComponent>("Score: " + std::to_string(totalScore), pFont);
			pScoreGO->m_RenderOrder = 1;
			scene.Add(std::move(pScoreGO));

			auto pPromptGO = std::make_unique<GameObject>();
			pPromptGO->SetLocalPosition(CENTER_X, 120.f);
			pPromptGO->AddComponent<TextComponent>("Enter name: W/S = letter, A/D = move", pFontSmall);
			pPromptGO->m_RenderOrder = 1;
			scene.Add(std::move(pPromptGO));

			// Name display GO — updated every frame
			auto pNameGO = std::make_unique<GameObject>();
			pNameGO->SetLocalPosition(CENTER_X + 20.f, 170.f);
			pNameGO->AddComponent<TextComponent>("AAA", pFont);
			pNameGO->m_RenderOrder = 1;
			m_pNameDisplay = pNameGO.get();
			scene.Add(std::move(pNameGO));

			// Cursor marker (simple > indicator)
			auto pCursorGO = std::make_unique<GameObject>();
			pCursorGO->SetLocalPosition(CENTER_X + 20.f, 210.f);
			pCursorGO->AddComponent<TextComponent>("^", pFont);
			pCursorGO->m_RenderOrder = 1;
			m_pCursorMarker = pCursorGO.get();
			scene.Add(std::move(pCursorGO));

			// Leaderboard header
			const std::string MODE_STR =
				(m_Data.mode == GameMode::SinglePlayer) ? "Singleplayer" : "Coop";

			auto pHeaderGO = std::make_unique<GameObject>();
			pHeaderGO->SetLocalPosition(CENTER_X, 260.f);
			pHeaderGO->AddComponent<TextComponent>("Top 10 - " + MODE_STR, pFontSmall);
			pHeaderGO->m_RenderOrder = 1;
			scene.Add(std::move(pHeaderGO));

			const auto& ENTRIES = ScoreManager::GetInstance().GetEntries(m_Data.mode);
			for (int i = 0; i < static_cast<int>(ENTRIES.size()); ++i)
			{
				const auto& E = ENTRIES[i];
				const std::string LINE =
					std::to_string(i + 1) + ". " + E.name + "  " + std::to_string(E.score);

				auto pEntryGO = std::make_unique<GameObject>();
				pEntryGO->SetLocalPosition(CENTER_X, 290.f + static_cast<float>(i) * 28.f);
				pEntryGO->AddComponent<TextComponent>(LINE, pFontSmall);
				pEntryGO->m_RenderOrder = 1;
				scene.Add(std::move(pEntryGO));
			}
		}

		// Music
		auto& pSound = ServiceLocator::GetSoundSystem();
		m_MusicId = pSound.AddSound("Data/Sounds/music.mp3");
		pSound.PlayLoop(m_MusicId, 0.05f);

		// Input — only keyboard confirm for now; full input handled in Update
		auto& input = InputManager::GetInstance();
		input.BindKeyboardCommand(SDL_SCANCODE_F2, InputManager::KeyState::Down,
			std::make_unique<MuteCommand>());
	}

	void ScoreScreenState::Exit()
	{
		ServiceLocator::GetSoundSystem().Stop(m_MusicId);
		InputManager::GetInstance().UnbindAll();
		SceneManager::GetInstance().RemoveActiveScene();
		m_pNameDisplay = nullptr;
		m_pCursorMarker = nullptr;
	}

	void ScoreScreenState::Update(float deltaTime)
	{
		if (m_Confirmed) return;
		m_InputTimer -= deltaTime;
		if (m_InputTimer > 0.f) return;

		HandleInput();
	}

	void ScoreScreenState::Render() const {}

	void ScoreScreenState::HandleInput()
	{
		const auto& INPUT = InputManager::GetInstance();

		// Helper lambdas to check both keyboard and controller
		auto isUp = [&]()
			{
				return INPUT.IsKeyboardKeyDown(SDL_SCANCODE_W)
					|| INPUT.IsControllerButtonDown(0, Controller::Button::DPadUp)
					|| INPUT.IsControllerButtonDown(1, Controller::Button::DPadUp);
			};

		auto isDown = [&]()
			{
				return INPUT.IsKeyboardKeyDown(SDL_SCANCODE_S)
					|| INPUT.IsControllerButtonDown(0, Controller::Button::DPadDown)
					|| INPUT.IsControllerButtonDown(1, Controller::Button::DPadDown);
			};

		auto isLeft = [&]()
			{
				return INPUT.IsKeyboardKeyDown(SDL_SCANCODE_A)
					|| INPUT.IsControllerButtonDown(0, Controller::Button::DPadLeft)
					|| INPUT.IsControllerButtonDown(1, Controller::Button::DPadLeft);
			};

		auto isRight = [&]()
			{
				return INPUT.IsKeyboardKeyDown(SDL_SCANCODE_D)
					|| INPUT.IsControllerButtonDown(0, Controller::Button::DPadRight)
					|| INPUT.IsControllerButtonDown(1, Controller::Button::DPadRight);
			};

		auto isConfirm = [&]()
			{
				return INPUT.IsKeyboardKeyDown(SDL_SCANCODE_RETURN)
					|| INPUT.IsControllerButtonDown(0, Controller::Button::ButtonA)
					|| INPUT.IsControllerButtonDown(1, Controller::Button::ButtonA);
			};

		if (m_Data.mode == GameMode::PvP)
		{
			if (isConfirm())
				m_GSM.SwitchTo(std::make_unique<MenuState>(m_GSM, m_WindowWidth, m_WindowHeight));
			return;
		}

		bool inputConsumed = false;

		if (isUp())
		{
			m_Letters[m_CursorPos] = (m_Letters[m_CursorPos] + 1) % NUM_CHARS;
			inputConsumed = true;
		}
		else if (isDown())
		{
			m_Letters[m_CursorPos] = (m_Letters[m_CursorPos] - 1 + NUM_CHARS) % NUM_CHARS;
			inputConsumed = true;
		}
		else if (isRight())
		{
			if (m_CursorPos < NAME_LENGTH - 1)
			{
				++m_CursorPos;
				inputConsumed = true;
			}
			else
			{
				Confirm();
				return;
			}
		}
		else if (isLeft())
		{
			if (m_CursorPos > 0)
			{
				--m_CursorPos;
				inputConsumed = true;
			}
		}
		else if (isConfirm())
		{
			Confirm();
			return;
		}

		if (inputConsumed)
		{
			m_InputTimer = INPUT_COOLDOWN;
			UpdateNameDisplay();
		}
	}

	void ScoreScreenState::Confirm()
	{
		std::string name;
		for (int i = 0; i < NAME_LENGTH; ++i)
			name += static_cast<char>('A' + m_Letters[i]);

		int totalScore = 0;
		for (int s : m_Data.playerScores) totalScore += s;

		ScoreEntry entry;
		entry.name = name;
		entry.score = totalScore;
		entry.mode = m_Data.mode;

		ScoreManager::GetInstance().AddEntry(entry);
		ScoreManager::GetInstance().Save();

		m_Confirmed = true;
		m_GSM.SwitchTo(std::make_unique<MenuState>(m_GSM, m_WindowWidth, m_WindowHeight));
	}

	void ScoreScreenState::UpdateNameDisplay()
	{
		if (!m_pNameDisplay) return;

		std::string name;
		for (int i = 0; i < NAME_LENGTH; ++i)
			name += static_cast<char>('A' + m_Letters[i]);

		if (auto* pText = m_pNameDisplay->GetComponent<TextComponent>())
			pText->SetText(name);

		// Move cursor marker under the active letter (each char ~22px wide at size 36)
		if (m_pCursorMarker)
		{
			constexpr float CHAR_WIDTH = 22.f;
			const float CENTER_X = m_WindowWidth * 0.5f - 100.f;
			m_pCursorMarker->SetLocalPosition(
				CENTER_X + 20.f + static_cast<float>(m_CursorPos) * CHAR_WIDTH,
				210.f);
		}
	}
}