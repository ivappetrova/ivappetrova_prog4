#include "TakeDmgState.h"
#include "IdleState.h"
#include "MoveState.h"
#include "JumpState.h"
#include "Components/PlayerComponent.h"
#include <iostream>

namespace dae
{
	void TakeDmgState::Enter(PlayerComponent& player)
	{
		player.SetInvincible(INVINCIBLE_TIME);
		player.TakeDamage(); 
		player.ApplyKnockback(KNOCKBACK_VX);
		m_Timer = INVINCIBLE_TIME;
	}

	PlayerState* TakeDmgState::HandleInput(PlayerComponent& player)
	{
		if (m_Timer <= 0.f) return new IdleState{};
		if (player.WantsJump() && player.IsGrounded()) return new JumpState{};
		if (player.GetMoveDirX() != 0.f) return new MoveState{};
		return nullptr;
	}

	void TakeDmgState::Update(PlayerComponent& player, float deltaTime)
	{
		m_Timer -= deltaTime;
		player.RequestMove(player.GetMoveDirX());
	}

	void TakeDmgState::Exit(PlayerComponent& player)
	{
		player.StopHorizontal();
	}
}