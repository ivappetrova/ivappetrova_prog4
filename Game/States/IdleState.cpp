#include "IdleState.h"
#include "MoveState.h"
#include "JumpState.h"
#include "ShootState.h"
#include "Components/PlayerComponent.h"
#include <iostream>

namespace dae
{
	void IdleState::Enter(PlayerComponent& PlayerComponent)
	{
		std::cout << "Entered IdleState\n";
		PlayerComponent.StopMove();
	}

	PlayerState* IdleState::HandleInput(PlayerComponent& PlayerComponent)
	{
		if (PlayerComponent.WantsShoot()) return new ShootState{ PlayerComponent.GetScene(), false };
		if (PlayerComponent.WantsJump() && PlayerComponent.IsGrounded()) return new JumpState{};
		if (PlayerComponent.GetMoveDirX() != 0.f) return new MoveState{};
		return nullptr;
	}

	void IdleState::Update(PlayerComponent& /*PlayerComponent*/, float /*deltaTime*/)
	{
		//Update sprite
	}
}