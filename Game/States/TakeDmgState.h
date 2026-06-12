#ifndef TAKE_DMG_STATE
#define TAKE_DMG_STATE

#include "PlayerState.h"

namespace dae
{
	class TakeDmgState final : public PlayerState
	{
	public:
		void Enter(PlayerComponent& PlayerComponent) override;
		PlayerState* HandleInput(PlayerComponent& PlayerComponent) override;
		void Update(PlayerComponent& PlayerComponent, float deltaTime) override;
		void Exit(PlayerComponent& PlayerComponent) override;

	private:
		float m_Timer{ 0.5f };
		static constexpr float KNOCKBACK_VX{ -150.f };
	};
}
#endif