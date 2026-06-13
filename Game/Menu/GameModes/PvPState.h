#ifndef PVPSTATE_H
#define PVPSTATE_H

#include "IGameState.h"
#include "SoundSystem.h"

namespace dae
{
	class GameStateManager;

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
	};
}
#endif