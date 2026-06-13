#include "Components/BubbleProjectileComponent.h"

#include "Components/BoxColliderComponent.h"
#include "Components/EnemyComponent.h"
#include "Components/PickUpComponent.h"
#include "Components/PlayerComponent.h"
#include "Components/PhysicsComponent.h"
#include "Components/LevelCollisionComponent.h"
#include "Utils/EnemyType.h"

#include "Scene.h"
#include "GameObject.h"
#include "TextureComponent.h"

#include <iostream>
#include <memory>

namespace dae
{
	BubbleProjectileComponent::BubbleProjectileComponent(GameObject* owner, Scene& scene, float directionX, [[maybe_unused]] GameObject* shooter, float windowHeight, LevelCollisionComponent* pLevelCol)
		: Component(owner), m_DirX(directionX >= 0.f ? 1.f : -1.f), m_Scene(scene), m_WindowHeight(windowHeight), m_pLevelCol(pLevelCol)
	{
		m_pCollider = owner->GetComponent<BoxColliderComponent>();
	}

	void BubbleProjectileComponent::Update(float deltaTime)
	{
		switch (m_Phase)
		{
		case Phase::Traveling:
		{
			UpdateTraveling(deltaTime);
			break;
		}
		case Phase::Trapped:
		{
			UpdateTrapped(deltaTime);
			break;
		}
		case Phase::Empty:
		{
			UpdateEmpty(deltaTime);
			break;
		}
		}
	}

	void BubbleProjectileComponent::UpdateTraveling(float dt)
	{
		// Move horizontally
		auto pos{ GetOwner()->GetWorldPosition() };
		pos.x += m_DirX * TRAVEL_SPEED * dt;
		GetOwner()->SetLocalPosition(pos.x, pos.y);

		// Check for enemy hit
		if (EnemyComponent* pEnemy = FindOverlappingEnemy())
		{
			m_pTrappedEnemy = pEnemy->GetOwner();

			// Hide the enemy (make it invisible and disable its physics)
			m_pTrappedEnemy->SetActive(false);

			m_Phase = Phase::Trapped;
			m_Timer = TRAP_TIMEOUT;
			std::cout << "[Bubble] Trapped enemy: " << pEnemy->GetEnemyType()->GetId() << std::endl;
			return;
		}

		// Travel timer expired = become an empty bubble
		m_Timer -= dt;
		if (m_Timer <= 0.f)
		{
			m_Phase = Phase::Empty;
			m_Timer = EMPTY_LIFETIME;
		}
	}

	void BubbleProjectileComponent::UpdateTrapped(float dt)
	{
		// Float upward
		auto pos{ GetOwner()->GetWorldPosition() };
		pos.y -= FLOAT_SPEED * dt;
		GetOwner()->SetLocalPosition(pos.x, pos.y);

		// Player burst check
		if (AnyPlayerOverlaps())
		{
			BurstBubble();
			return;
		}

		// Timeout = release enemy
		m_Timer -= dt;
		if (m_Timer <= 0.f)
		{
			ReleaseTrappedEnemy();
		}
	}

	void BubbleProjectileComponent::UpdateEmpty(float dt)
	{
		// Float upward while the bubble lingers
		auto pos{ GetOwner()->GetWorldPosition() };
		pos.y -= FLOAT_SPEED * dt;
		GetOwner()->SetLocalPosition(pos.x, pos.y);

		m_Timer -= dt;
		if (m_Timer <= 0.f)
		{
			GetOwner()->MarkForDestroy();
		}
	}

	EnemyComponent* BubbleProjectileComponent::FindOverlappingEnemy() const
	{
		if (!m_pCollider) return nullptr;

		for (const auto& pGameObj : m_Scene.GetGameObjects())
		{
			if (!pGameObj || !pGameObj->IsActive()) continue;
			if (pGameObj.get() == GetOwner()) continue;

			EnemyComponent* pEnemy = pGameObj->GetComponent<EnemyComponent>();
			if (!pEnemy || !pEnemy->IsAlive()) continue;

			BoxColliderComponent* pOtherCol = pGameObj->GetComponent<BoxColliderComponent>();
			if (!pOtherCol) continue;

			// ← add this
			std::cout << "Bubble TL: " << m_pCollider->GetTopLeft().x << "," << m_pCollider->GetTopLeft().y
				<< " Enemy TL: " << pOtherCol->GetTopLeft().x << "," << pOtherCol->GetTopLeft().y << "\n";

			if (m_pCollider->Overlaps(*pOtherCol))
				return pEnemy;
		}
		return nullptr;
	}

	bool BubbleProjectileComponent::AnyPlayerOverlaps() const
	{
		if (!m_pCollider) return false;

		for (const auto& pGameObj : m_Scene.GetGameObjects())
		{
			if (!pGameObj || !pGameObj->IsActive()) continue;
			if (!pGameObj->GetComponent<PlayerComponent>()) continue;

			BoxColliderComponent* pCol = pGameObj->GetComponent<BoxColliderComponent>();
			if (pCol && m_pCollider->Overlaps(*pCol))
				return true;
		}
		return false;
	}

	void BubbleProjectileComponent::BurstBubble()
	{
		if (!m_pTrappedEnemy) return;

		auto* pEnemy = m_pTrappedEnemy->GetComponent<EnemyComponent>();
		if (!pEnemy) return;

		const EnemyType* pENEMY_TYPE = pEnemy->GetEnemyType();

		// Determine food texture and point value by enemy type id
		std::string foodTexture;
		int points{};

		if (pENEMY_TYPE->GetId() == "ZenChan")
		{
			foodTexture = "Characters/ZenChan/Watermelon.png";
			points = 100;
		}
		else if (pENEMY_TYPE->GetId() == "Maita")
		{
			foodTexture = "Characters/Maita/Fries.png";
			points = 200;
		}
		else
		{
			// use watermelon for unknown enemy types
			foodTexture = "Characters/ZenChan/Watermelon.png";
			points = 100;
		}

		// Kill the enemy and remove it from the scene
		pEnemy->Kill();
		m_pTrappedEnemy->MarkForDestroy();
		m_pTrappedEnemy = nullptr;

		// Spawn food pickup at the bubble's current position
		const auto BUBBLE_POS = GetOwner()->GetWorldPosition();

		auto foodGO = std::make_unique<GameObject>();
		foodGO->SetLocalPosition(BUBBLE_POS.x, BUBBLE_POS.y);

		constexpr float FOOD_SIZE{ 32.f };
		foodGO->AddComponent<TextureComponent>(FOOD_SIZE, FOOD_SIZE) ->SetTexture(foodTexture);
		foodGO->AddComponent<BoxColliderComponent>(FOOD_SIZE, FOOD_SIZE);
		auto* pPhysics = foodGO->AddComponent<PhysicsComponent>(m_WindowHeight);
		pPhysics->SetVelocityY(-200.f);
		if (m_pLevelCol) pPhysics->SetLevelCollision(m_pLevelCol);

		foodGO->AddComponent<PickUpComponent>(m_Scene, points);

		m_Scene.Add(std::move(foodGO));

		std::cout << "[Bubble] Burst! Spawned " << foodTexture << " worth " << points << " pts.\n";

		// Self-destruct
		GetOwner()->MarkForDestroy();
	}

	void BubbleProjectileComponent::ReleaseTrappedEnemy()
	{
		if (m_pTrappedEnemy)
		{
			const auto BUBBLE_POS = GetOwner()->GetWorldPosition();

			constexpr float CEILING_Y{ 60.f };
			float spawnY = BUBBLE_POS.y;

			if (auto* col = m_pTrappedEnemy->GetComponent<BoxColliderComponent>())
			{
				const float MIN_SAFE_Y = CEILING_Y + col->GetHeight() + 1.f;
				if (spawnY < MIN_SAFE_Y)
				{
					spawnY = MIN_SAFE_Y;
				}
			}

			m_pTrappedEnemy->SetLocalPosition(BUBBLE_POS.x, spawnY);

			if (auto* phys = m_pTrappedEnemy->GetComponent<PhysicsComponent>())
			{
				phys->SetVelocityY(0.f);
				phys->SetVelocityX(0.f);
			}

			m_pTrappedEnemy->SetActive(true);
			m_pTrappedEnemy = nullptr;
			std::cout << "[Bubble] Enemy escaped!\n";
		}
		GetOwner()->MarkForDestroy();
	}

}