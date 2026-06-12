#include "EnemyComponent.h"
#include <cassert>
#include "GameObject.h" 

namespace dae
{
    EnemyComponent::EnemyComponent(GameObject* owner, const EnemyType* pType)
        : Component(owner)
        , m_pType{ pType }
    {
        assert(pType && "EnemyComponent requires a valid EnemyType pointer");
    }

    void EnemyComponent::Update(float /*deltaTime*/)
    {
        // No behaviour yet.
        // AI state machine goes here in a future sprint.
    }

	void EnemyComponent::Kill()
	{
		m_IsAlive = false;
		GetOwner()->MarkForDestroy();
	}

}
