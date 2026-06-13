#include "Components/PvPBubbleProjectileComponent.h"
#include "Components/BoxColliderComponent.h"
#include "Components/PlayerComponent.h"

#include "Scene.h"
#include "GameObject.h"

namespace dae
{
	PvPBubbleProjectileComponent::PvPBubbleProjectileComponent(GameObject* owner, Scene& scene, float directionX, GameObject* shooter)
		: Component(owner) , m_Scene(scene) , m_DirX(directionX >= 0.f ? 1.f : -1.f) , m_pShooter(shooter)
	{
		m_pCollider = owner->GetComponent<BoxColliderComponent>();
	}

	void PvPBubbleProjectileComponent::Update(float deltaTime)
	{
		UpdateTraveling(deltaTime);
	}

	void PvPBubbleProjectileComponent::UpdateTraveling(float dt)
	{
		// Move horizontally
		auto pos = GetOwner()->GetWorldPosition();
		pos.x += m_DirX * TRAVEL_SPEED * dt;
		GetOwner()->SetLocalPosition(pos.x, pos.y);

		if (m_pCollider)
		{
			for (const auto& pGO : m_Scene.GetGameObjects())
			{
				if (!pGO || !pGO->IsActive()) continue;
				if (pGO.get() == GetOwner()) continue;
				if (pGO.get() == m_pShooter) continue;

				auto* pPlayer = pGO->GetComponent<PlayerComponent>();
				if (!pPlayer) continue;
				if (pPlayer->IsInvincible()) continue;

				auto* pCol = pGO->GetComponent<BoxColliderComponent>();
				if (!pCol) continue;

				if (m_pCollider->Overlaps(*pCol))
				{
					pPlayer->SetInvincible(1.5f);
					pPlayer->TakeDamage();
					GetOwner()->MarkForDestroy();
					return;
				}
			}
		}

		// Expire after travel time
		m_Timer -= dt;
		if (m_Timer <= 0.f)
			GetOwner()->MarkForDestroy();
	}
}