#pragma once
#include "GameObjectCommand.h"
#include "HealthComponent.h"

namespace dae
{
	class DealDamageCommand final : public GameObjectCommand
	{
	public:
		DealDamageCommand(GameObject* target, int damage, GameObject* attacker = nullptr)
			: GameObjectCommand(target), m_Damage(damage), m_pAttacker(attacker) {
		}

		void Execute() override
		{
			if (auto* health = GetGameObject()->GetComponent<HealthComponent>())
				health->TakeDamage(m_Damage, m_pAttacker);
		}

	private:
		int m_Damage;
		GameObject* m_pAttacker;
	};
}