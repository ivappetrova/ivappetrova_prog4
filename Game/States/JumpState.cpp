#include "JumpState.h"
#include "FallState.h"
#include "ShootState.h"
#include "Player.h"
#include <iostream>

namespace dae
{
	void JumpState::Enter(Player& /*player*/)
	{
		std::cout << "Entered JumpState" << std::endl;
	}

	PlayerState* JumpState::HandleInput(Player& player)
	{
		if (player.WantsShoot())
		{
			return new ShootState{ false };
		}
		if (player.GetVelocityY() >= 0.f)
		{
			return new FallState{};
		}

		return nullptr;
	}

	void JumpState::Update(Player& /*player*/, float /*deltaTime*/) 
	{
	}
}