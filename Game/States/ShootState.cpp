#include "ShootState.h"
#include "IdleState.h"
#include "MoveState.h"
#include "Player.h"
#include <iostream>

namespace dae
{
	ShootState::ShootState(bool wasMoving)
		: m_WasMoving(wasMoving) {}

	void ShootState::Enter(Player& /*player*/)
	{
		m_Timer = SHOOT_DURATION;
		// TODO: spawn projectile here
		std::cout << "Entered ShootState" << std::endl;
	}

	PlayerState* ShootState::HandleInput(Player& player)
	{
		if (m_Timer <= 0.f)
		{
			if (m_WasMoving && player.GetMoveDirX() != 0.f)
			{
				return new MoveState{};
			}
			return new IdleState{};
		}
		return nullptr;
	}

	void ShootState::Update(Player& /*player*/, float deltaTime)
	{
		m_Timer -= deltaTime;
	}
}