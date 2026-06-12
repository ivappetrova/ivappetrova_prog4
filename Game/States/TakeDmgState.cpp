#include "TakeDmgState.h"
#include "IdleState.h"
#include "Components/PlayerComponent.h"
#include <iostream>

namespace dae
{
	void TakeDmgState::Enter(PlayerComponent& PlayerComponent)
	{
		std::cout << "Entered TakeDmgState\n";
		PlayerComponent.ApplyKnockback(KNOCKBACK_VX);
	}

	PlayerState* TakeDmgState::HandleInput(PlayerComponent& /*PlayerComponent*/)
	{
		if (m_Timer <= 0.f) return new IdleState{};
		return nullptr;
	}

	void TakeDmgState::Update(PlayerComponent& PlayerComponent, float deltaTime)
	{
		// Physics integrates position — we only manage the velocity decay here
		PlayerComponent.DecayHorizontalVelocity(deltaTime);
		m_Timer -= deltaTime;
	}

	void TakeDmgState::Exit(PlayerComponent& PlayerComponent)
	{
		PlayerComponent.StopHorizontal();
	}
}