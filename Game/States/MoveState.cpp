#include "MoveState.h"
#include "IdleState.h"
#include "JumpState.h"
#include "ShootState.h"
#include "Components/PlayerComponent.h"
#include <iostream>

namespace dae
{
	void MoveState::Enter(PlayerComponent& /*PlayerComponent*/)
	{
		std::cout << "Entered MoveState\n";
	}

	PlayerState* MoveState::HandleInput(PlayerComponent& PlayerComponent)
	{
		if (PlayerComponent.WantsShoot())  return new ShootState{ PlayerComponent.GetScene(), true };
		if (PlayerComponent.WantsJump() && PlayerComponent.IsGrounded()) return new JumpState{};
		if (PlayerComponent.GetMoveDirX() == 0.f) return new IdleState{};
		return nullptr;
	}

	void MoveState::Update(PlayerComponent& PlayerComponent, float /*deltaTime*/)
	{
		PlayerComponent.RequestMove(PlayerComponent.GetMoveDirX());
	}

	void MoveState::Exit(PlayerComponent& PlayerComponent)
	{
		PlayerComponent.StopMove();
	}
}