#pragma once
#ifndef TAKEDMGSTATE_H
#define TAKEDMGSTATE_H

#include "PlayerState.h"

namespace dae
{
	class TakeDmgState final : public PlayerState
	{
	public:
		void Enter(PlayerComponent& player)  override;
		void Exit(PlayerComponent& player)   override;
		PlayerState* HandleInput(PlayerComponent& player) override;
		void Update(PlayerComponent& player, float deltaTime) override;

	private:
		static constexpr float KNOCKBACK_VX{ -200.f };
		static constexpr float INVINCIBLE_TIME{ 3.f };

		float m_Timer{ INVINCIBLE_TIME };
	};
}
#endif