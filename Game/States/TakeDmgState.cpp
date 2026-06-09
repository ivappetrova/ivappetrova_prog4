#include "TakeDmgState.h"
#include "IdleState.h"
#include "Player.h"
#include "GameObject.h"
#include <iostream>

namespace dae
{
	void TakeDmgState::Enter(Player& player)
	{
		player.SetVelocity(KNOCKBACK_VX, 0.f);
		std::cout << "Entered TakeDmgState" << std::endl;
	}

	PlayerState* TakeDmgState::HandleInput(Player& /*player*/)
	{
		if (m_Timer <= 0.f)
		{
			return new IdleState{};
		}
			
		return nullptr;
	}

	void TakeDmgState::Update(Player& player, float deltaTime)
	{
		GameObject* pGO = player.GetGameObject();
		const auto& POS = pGO->GetWorldPosition();
		pGO->SetLocalPosition(POS.x + player.GetVelocityX() * deltaTime, POS.y);

		m_Timer -= deltaTime;
	}
}