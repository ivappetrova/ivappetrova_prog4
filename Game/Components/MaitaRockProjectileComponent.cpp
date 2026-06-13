#include "MaitaRockProjectileComponent.h"
#include "GameObject.h"
#include "Components/BoxColliderComponent.h"
#include "Components/HealthComponent.h"
#include "Components/PlayerComponent.h"
#include "LevelCollisionComponent.h"
#include "Scene.h"
#include <glm/vec2.hpp>

namespace dae
{
	MaitaRockProjectileComponent::MaitaRockProjectileComponent( GameObject* owner, Scene& scene, float dirX, LevelCollisionComponent* pLevelCol)
		: Component(owner) , m_Scene(scene) , m_DirX(dirX) , m_pLevelCol(pLevelCol)
	{
	}

	void MaitaRockProjectileComponent::Update(float deltaTime)
	{
		m_Lifetime -= deltaTime;
		if (m_Lifetime <= 0.f)
		{
			GetOwner()->MarkForDestroy();
			return;
		}

		auto* pOwner = GetOwner();
		auto pos = pOwner->GetWorldPosition();
		const float newX = pos.x + m_DirX * m_Speed * deltaTime;
		pOwner->SetLocalPosition(newX, pos.y);

		// Wall check
		auto* pBox = pOwner->GetComponent<BoxColliderComponent>();
		if (m_pLevelCol && pBox)
		{
			const bool IS_MOVING_RIGHT = m_DirX > 0.f;
			const float LEAD_X = IS_MOVING_RIGHT ? newX + pBox->GetWidth() : newX;
			const glm::vec2 SAMPLE{ LEAD_X, pos.y - pBox->GetHeight() * 0.5f };
			const float WALL = m_pLevelCol->WallXBeside(SAMPLE, m_Speed * deltaTime + 2.f, IS_MOVING_RIGHT);
			const bool HIT_WALL = IS_MOVING_RIGHT ? (WALL != FLT_MAX) : (WALL != -FLT_MAX);
			if (HIT_WALL)
			{
				pOwner->MarkForDestroy();
				return;
			}
		}

		// Player hit check
		auto* pMyBox = pOwner->GetComponent<BoxColliderComponent>();
		if (!pMyBox) return;

		const float MY_LEFT = newX;
		const float MY_RIGHT = newX + pMyBox->GetWidth();
		const float MY_TOP = pos.y - pMyBox->GetHeight();
		const float MY_BOT = pos.y;

		for (auto& go : m_Scene.GetGameObjects())
		{
			if (!go) continue;
			auto* pPlayer = go->GetComponent<PlayerComponent>();
			if (!pPlayer) continue;

			auto* pPlayerBox = go->GetComponent<BoxColliderComponent>();
			if (!pPlayerBox) continue;

			auto pPos = go->GetWorldPosition();
			const float P_LEFT = pPos.x;
			const float P_RIGHT = pPos.x + pPlayerBox->GetWidth();
			const float P_TOP = pPos.y - pPlayerBox->GetHeight();
			const float P_BOT = pPos.y;

			const bool OVERLAP = MY_RIGHT > P_LEFT && MY_LEFT < P_RIGHT
				&& MY_BOT   > P_TOP && MY_TOP < P_BOT;
			if (OVERLAP)
			{
				if (auto* pHP = go->GetComponent<HealthComponent>())
				{
					pHP->TakeDamage(1);
				}

				pOwner->MarkForDestroy();
				return;
			}
		}
	}
}