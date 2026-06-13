#ifndef SHOOT_STATE
#define SHOOT_STATE

#include "PlayerState.h"
#include "Scene.h"
#include "ISoundSystem.h"

namespace dae
{
	class ShootState final : public PlayerState
	{
	public:
		explicit ShootState(Scene& scene, bool wasMoving = false, sound_id shootSound = 0);

		void Enter(PlayerComponent& PlayerComponent) override;
		PlayerState* HandleInput(PlayerComponent& PlayerComponent) override;
		void Update(PlayerComponent& PlayerComponent, float deltaTime) override;

	private:
		Scene& m_Scene;
		float m_Timer{ 0.f };
		static constexpr float SHOOT_DURATION{ 0.3f };
		bool m_WasMoving{ false };
		sound_id m_ShootSound{};
	};
}
#endif