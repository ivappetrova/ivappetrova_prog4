#ifndef MULTIPLAYERSTATE_H
#define MULTIPLAYERSTATE_H

#include "IGameState.h"
#include "SoundSystem.h"

namespace dae
{
	class GameStateManager;

	class MultiplayerState final : public IGameState
	{
	public:
		MultiplayerState(GameStateManager& gsm, float windowWidth, float windowHeight);
		~MultiplayerState() override = default;

		void Enter()  override;
		void Exit()   override;
		void Update(float deltaTime) override;
		void Render() const override;

	private:
		GameStateManager& m_GSM;
		float m_WindowWidth;
		float m_WindowHeight;
		sound_id m_MusicId{};
	};
}
#endif