#ifndef ENEMY_COMPONENT_H
#define ENEMY_COMPONENT_H

#include "Component.h"
#include "Utils/EnemyType.h"

namespace dae
{

	class EnemyComponent final : public Component
	{
	public:
		// pType must outlive this component (the registry owns it)
		EnemyComponent(GameObject* owner, const EnemyType* pType);

		void Update(float deltaTime) override;

		const EnemyType* GetEnemyType() const { return m_pType; }
		GameObject* GetOwner() const { return Component::GetOwner(); }

		// Alive flag — set false when the player's bubble captures it
		bool IsAlive() const { return m_IsAlive; }
		void Kill();

		// Rule of 5
		EnemyComponent(const EnemyComponent&) = delete;
		EnemyComponent& operator=(const EnemyComponent&) = delete;
		EnemyComponent(EnemyComponent&&) = delete;
		EnemyComponent& operator=(EnemyComponent&&) = delete;

	private:
		const EnemyType* m_pType;
		bool m_IsAlive{ true };
	};

}
#endif