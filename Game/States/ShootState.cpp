#include "ShootState.h"
#include "IdleState.h"
#include "MoveState.h"
#include "Components/PlayerComponent.h"
#include <iostream>

namespace dae
{
	ShootState::ShootState(bool wasMoving)
		: m_WasMoving(wasMoving) {}

	void ShootState::Enter(PlayerComponent& PlayerComponent)
	{
		std::cout << "Entered ShootState\n";
		m_Timer = SHOOT_DURATION;
		PlayerComponent.StopMove();   // freeze horizontal while shooting
		// TODO: spawn projectile here
	}

	PlayerState* ShootState::HandleInput(PlayerComponent& PlayerComponent)
	{
		if (m_Timer <= 0.f)
		{
			if (m_WasMoving && PlayerComponent.GetMoveDirX() != 0.f)
				return new MoveState{};
			return new IdleState{};
		}
		return nullptr;
	}

	void ShootState::Update(PlayerComponent& /*PlayerComponent*/, float deltaTime)
	{
		m_Timer -= deltaTime;
	}
}