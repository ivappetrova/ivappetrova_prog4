#ifndef MULTIPLAYERSTATE_H
#define MULTIPLAYERSTATE_H

#include "IGameState.h"
#include "SoundSystem.h"
#include "Utils/SoundObserver.h"
#include <optional>

namespace dae
{
	class GameStateManager;

	class CoopState final : public IGameState
	{
	public:
		CoopState(GameStateManager& gsm, float windowWidth, float windowHeight);
		~CoopState() override = default;

		void Enter()  override;
		void Exit()   override;
		void Update(float deltaTime) override;
		void Render() const override;

	private:
		GameStateManager& m_GSM;
		float m_WindowWidth;
		float m_WindowHeight;
		sound_id m_MusicId{};

		std::optional<SoundObserver> m_HitObserver1;
		std::optional<SoundObserver> m_HitObserver2;
		std::optional<SoundObserver> m_PointObserver1;
		std::optional<SoundObserver> m_PointObserver2;
	};
}
#endif