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
		if (PlayerComponent.GetVelocityY() >= 0.f)
		{
			return new FallState{};
		}

		return nullptr;
	}

	void JumpState::Update(PlayerComponent& PlayerComponent, float /*deltaTime*/)
	{
		const float PLAYER_DIR = PlayerComponent.GetMoveDirX();
		if (PLAYER_DIR != 0.f)
		{
			PlayerComponent.RequestMove(PLAYER_DIR);
		}
		else
		{
			PlayerComponent.StopMove();
		}
	}
}