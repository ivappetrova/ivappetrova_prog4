#ifndef PVPSTATE_H
#define PVPSTATE_H

#include "IGameState.h"
#include "SoundSystem.h"
#include "Utils/SoundObserver.h"
#include <optional>

namespace dae
{
	class GameStateManager;
	class GameObject;

	class PvPState final : public IGameState
	{
	public:
		PvPState(GameStateManager& gsm, float windowWidth, float windowHeight);
		~PvPState() override = default;

		void Enter()  override;
		void Exit()   override;
		void Update(float deltaTime) override;
		void Render() const override;

	private:
		GameStateManager& m_GSM;
		float m_WindowWidth;
		float m_WindowHeight;
		sound_id m_MusicId{};
		GameObject* m_pChar1{ nullptr };
		GameObject* m_pChar2{ nullptr };
		bool m_GameOver{ false };
		std::optional<SoundObserver> m_HitObserver1;
		std::optional<SoundObserver> m_HitObserver2;
	};
}
#endif