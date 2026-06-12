#ifndef SHOOT_STATE
#define SHOOT_STATE

#include "PlayerState.h"
#include "Scene.h"

namespace dae
{
	class ShootState final : public PlayerState
	{
	public:
		explicit ShootState(Scene& scene, bool wasMoving = false);

		void Enter(PlayerComponent& PlayerComponent) override;
		PlayerState* HandleInput(PlayerComponent& PlayerComponent) override;
		void Update(PlayerComponent& PlayerComponent, float deltaTime) override;

	private:
		float m_Timer{ 0.f };
		bool  m_WasMoving{ false };
		Scene& m_Scene;
		static constexpr float SHOOT_DURATION{ 0.3f };
	};
}
#endif