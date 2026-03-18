#pragma once
#include "Component.h"
#include "Subject.h"
#include "Event.h"
#include "PlayerPointsComponent.h"

namespace dae
{
	class HealthComponent final : public Component, public Subject
	{
	public:
		HealthComponent(GameObject* owner, int maxHP = 3)
			: Component(owner), m_HP(maxHP), m_MaxHP(maxHP) {
		}

		void TakeDamage(int damage, GameObject* attacker = nullptr)
		{
			if (m_HP <= 0) return;
			m_HP -= damage;

			if (m_HP <= 0)
			{
				m_HP = 0;
				NotifyObservers(Event{ EVENT_PLAYER_DIED }, GetOwner());
				if (attacker)
					if (auto* pts = attacker->GetComponent<PlayerPointsComponent>())
						pts->AddPoints(200);
			}
			else
			{
				NotifyObservers(Event{ EVENT_PLAYER_HIT }, GetOwner());
				if (attacker)
					if (auto* pts = attacker->GetComponent<PlayerPointsComponent>())
						pts->AddPoints(50);
			}
		}

		int GetHP() const { return m_HP; }
		int GetMaxHP() const { return m_MaxHP; }

	private:
		int m_HP;
		int m_MaxHP;
	};
}