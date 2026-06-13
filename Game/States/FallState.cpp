#include "FallState.h"
#include "IdleState.h"
#include "Components/PlayerComponent.h"
#include <iostream>

namespace dae
{
	void FallState::Enter(PlayerComponent& /*PlayerComponent*/)
	{
		std::cout << "Entered FallState\n";
	}

	PlayerState* FallState::HandleInput(PlayerComponent& PlayerComponent)
	{
		if (PlayerComponent.IsGrounded()) return new IdleState{};
		return nullptr;
	}

	void FallState::Update(PlayerComponent& PlayerComponent, float /*deltaTime*/)
	{
		const float DIRECTION = PlayerComponent.GetMoveDirX();
		if (DIRECTION != 0.f)
		{
			PlayerComponent.RequestMove(DIRECTION);
		}
		else
		{
			PlayerComponent.StopMove();
		}
	}
}