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
		// Allow steering while falling
		const float dir = PlayerComponent.GetMoveDirX();
		if (dir != 0.f)
			PlayerComponent.RequestMove(dir);
		else
			PlayerComponent.StopMove();
	}
}