#pragma once
#include "GameObjectCommand.h"
#include "HealthComponent.h"

namespace dae
{
	class DealDamageCommand final : public GameObjectCommand
	{
	public:
		DealDamageCommand(GameObject* gameObject, int damage)
			: GameObjectCommand(gameObject), m_Damage(damage) 
		{
		}

		void Execute() override
		{
			if (auto* health = GetGameObject()->GetComponent<HealthComponent>())
				health->TakeDamage(m_Damage);
		}

	private:
		int m_Damage;
	};
}