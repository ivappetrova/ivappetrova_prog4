#include "IdleState.h"
#include "MoveState.h"
#include "JumpState.h"
#include "ShootState.h"
#include "Player.h"
#include <iostream>

namespace dae
{
	void IdleState::Enter(Player& /*player*/)
	{
		std::cout << "Entered IdleState" << std::endl;
	}

	PlayerState* IdleState::HandleInput(Player& player)
	{
		if (player.WantsShoot())
		{
			return new ShootState{ false };
		}
		if (player.WantsJump())
		{
			return new JumpState{};
		}
		if (player.GetMoveDirX() != 0.f)
		{
			return new MoveState{};
		}

		return nullptr;
	}

	void IdleState::Update(Player& /*player*/, float /*deltaTime*/) 
	{
	}
}