#include "Components/PickupComponent.h"

#include "Components/BoxColliderComponent.h"
#include "Components/PlayerComponent.h"
#include "Components/ScoreComponent.h"

#include "Scene.h"
#include "GameObject.h"

#include <iostream>

namespace dae
{
	PickupComponent::PickupComponent(GameObject* owner, Scene& scene, int points)
		: Component(owner)
		, m_Scene(scene)
		, m_Points(points)
	{
		m_pCollider = owner->GetComponent<BoxColliderComponent>();
	}

	void PickupComponent::Update(float deltaTime)	
	{
		if (m_SpawnDelay > 0.f)
		{
			m_SpawnDelay -= deltaTime;
			return; // not collectible yet
		}

		if (m_Collected || !m_pCollider) return;

		for (const auto& go : m_Scene.GetGameObjects())
		{
			if (!go || !go->IsActive()) continue;

			PlayerComponent* pPlayer = go->GetComponent<PlayerComponent>();
			if (!pPlayer) continue;

			BoxColliderComponent* pCol = go->GetComponent<BoxColliderComponent>();
			if (!pCol || !m_pCollider->Overlaps(*pCol)) continue;

			ScoreComponent* pScore = go->GetComponent<ScoreComponent>();
			if (pScore)
			{
				pScore->AddPoints(m_Points);
				std::cout << "[Pickup] Player collected " << m_Points << " pts.\n";
			}

			m_Collected = true;
			GetOwner()->MarkForDestroy();
			return;
		}
	}

} // namespace dae