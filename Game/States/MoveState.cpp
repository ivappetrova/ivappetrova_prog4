#include "MoveState.h"
#include "IdleState.h"
#include "JumpState.h"
#include "ShootState.h"
#include "Player.h"
#include "GameObject.h"
#include <iostream>

namespace dae
{
	void MoveState::Enter(Player& /*player*/)
	{
		std::cout << "Entered MoveState" << std::endl;
	}

	PlayerState* MoveState::HandleInput(Player& player)
	{
		if (player.WantsShoot())
		{
			return new ShootState{ true };
		}
		if (player.WantsJump())
		{
			return new JumpState{};
		}	
		if (player.GetMoveDirX() == 0.f)
		{
			return new IdleState{};
		}
			
		return nullptr;
	}

	void MoveState::Update(Player& player, float deltaTime)
	{
		player.SetVelocity(player.GetMoveDirX() * WALK_SPEED, 0.f);

		GameObject* gameObj = player.GetGameObject();
		const auto& POS = gameObj->GetWorldPosition();
		gameObj->SetLocalPosition(POS.x + player.GetVelocityX() * deltaTime, POS.y);
	}
}