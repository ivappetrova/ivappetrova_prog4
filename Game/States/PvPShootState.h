#ifndef PVP_SHOOT_STATE_H
#define PVP_SHOOT_STATE_H

#include "PlayerState.h"
#include "Scene.h"
#include "ISoundSystem.h"

namespace dae
{
	class PvPShootState final : public PlayerState
	{
	public:
		explicit PvPShootState(Scene& scene, bool wasMoving = false, sound_id shootSound = 0);

		void Enter(PlayerComponent& player) override;
		PlayerState* HandleInput(PlayerComponent& player) override;
		void Update(PlayerComponent& player, float deltaTime) override;

	private:
		Scene& m_Scene;
		float m_Timer{ 0.f };
		bool m_WasMoving{ false };
		sound_id m_ShootSound{};

		static constexpr float SHOOT_DURATION{ 0.3f };
	};
}
#endif