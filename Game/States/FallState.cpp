#include "FallState.h"
#include "IdleState.h"
#include "Player.h"
#include <iostream>

namespace dae
{
	void FallState::Enter(Player& /*player*/)
	{
		std::cout << "Entered FallState" << std::endl;
	}

	PlayerState* FallState::HandleInput(Player& player)
	{
		if (player.IsGrounded())
		{ 
			return new IdleState{};
		}

		return nullptr;
	}

	void FallState::Update(Player& /*player*/, float /*deltaTime*/) 
	{
	}
}