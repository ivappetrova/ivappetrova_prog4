#ifndef SCORE_SCREEN_STATE_H
#define SCORE_SCREEN_STATE_H

#include "IGameState.h"
#include "ScoreScreenData.h"
#include "SoundSystem.h"
#include <array>
#include <vector>
#include <string>

namespace dae
{
	class GameStateManager;
	class GameObject;

	class ScoreScreenState final : public IGameState
	{
	public:
		ScoreScreenState(GameStateManager& gsm, float windowWidth, float windowHeight, ScoreScreenData data);
		~ScoreScreenState() override = default;

		void Enter() override;
		void Exit() override;
		void Update(float deltaTime) override;
		void Render() const          override;

	private:
		void HandleInput();
		void Confirm();
		void UpdateNameDisplay();

		static constexpr int   NAME_LENGTH = 3;
		static constexpr int   NUM_CHARS = 26;
		static constexpr float INPUT_COOLDOWN = 0.15f;

		GameStateManager& m_GSM;
		float m_WindowWidth;
		float m_WindowHeight;
		ScoreScreenData m_Data;

		int m_ActivePlayer{ 0 };
		int m_CursorPos{ 0 };
		std::array<int, NAME_LENGTH> m_Letters{ 0, 0, 0 }; // 0 = A
		bool m_Confirmed{ false };

		float m_InputTimer{ 0.f };

		// Scene objects
		GameObject* m_pNameDisplay{ nullptr };
		GameObject* m_pCursorMarker{ nullptr };

		sound_id m_MusicId{};
	};
}
#endif