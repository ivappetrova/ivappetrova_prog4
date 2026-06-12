#include "JumpState.h"
#include "FallState.h"
#include "ShootState.h"
#include "Components/PlayerComponent.h"
#include <iostream>

namespace dae
{
	void JumpState::Enter(PlayerComponent& PlayerComponent)
	{
		std::cout << "Entered JumpState" << std::endl;
		PlayerComponent.Jump();
	}

	PlayerState* JumpState::HandleInput(PlayerComponent& PlayerComponent)
	{
		if (PlayerComponent.WantsShoot())
		{
			return new ShootState{ false };
		}
		if (PlayerComponent.GetVelocityY() >= 0.f)
		{
			return new FallState{};
		}

		return nullptr;
	}

	void JumpState::Update(PlayerComponent& PlayerComponent, float /*deltaTime*/)
	{
		const float dir = PlayerComponent.GetMoveDirX();
		if (dir != 0.f)
			PlayerComponent.RequestMove(dir);
		else
			PlayerComponent.StopMove();
	}
}