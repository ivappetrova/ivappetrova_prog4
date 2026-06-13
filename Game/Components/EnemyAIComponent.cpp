#include "EnemyAIComponent.h"

#include "GameObject.h"
#include "Scene.h"
#include "TextureComponent.h"

#include "Components/PhysicsComponent.h"
#include "Components/BoxColliderComponent.h"
#include "Components/BoxColliderDebugDrawComponent.h"
#include "Components/PlayerComponent.h"
#include "Components/LevelCollisionComponent.h"
#include "Components/MaitaRockProjectileComponent.h"

#include <cfloat>
#include <cmath>

namespace dae
{
	EnemyAIComponent::EnemyAIComponent(GameObject* owner, Scene& scene, const std::string& typeId, float /*windowHeight*/)
		: Component(owner), m_Scene(scene), m_TypeId(typeId)
	{
		m_JumpCooldown = JUMP_INTERVAL * (0.3f + static_cast<float>(rand() % 100) / 200.f);
		m_ShootCooldown = SHOOT_INTERVAL * (0.5f + static_cast<float>(rand() % 100) / 200.f);
		m_PatrolDir = (rand() % 2 == 0) ? 1.f : -1.f;
		m_DirChangeCooldown = DIR_CHANGE_INTERVAL * (0.5f + static_cast<float>(rand() % 100) / 100.f);
		m_LastPosX = owner->GetWorldPosition().x;
	}

	void EnemyAIComponent::Update(float deltaTime)
	{
		if (m_TypeId == "ZenChan")
			UpdateZenChan(deltaTime);
		else if (m_TypeId == "Maita")
			UpdateMaita(deltaTime);
	}

	void EnemyAIComponent::UpdateZenChan(float deltaTime)
	{
		auto* pPhys = GetOwner()->GetComponent<PhysicsComponent>();
		if (!pPhys) return;

		// Track position to detect actual wall blocking
		const auto CURRENT_POS = GetOwner()->GetWorldPosition();
		const float MOVED_X = std::abs(CURRENT_POS.x - m_LastPosX);
		const float EXPECTED_MOVE = MOVE_SPEED * deltaTime;

		m_FlipCooldown -= deltaTime;
		if (m_FlipCooldown <= 0.f && MOVED_X < EXPECTED_MOVE * 0.25f && !pPhys->IsGrounded() == false)
		{
			m_PatrolDir = -m_PatrolDir;
			m_FlipCooldown = FLIP_COOLDOWN;
		}

		m_LastPosX = CURRENT_POS.x;

		m_DirChangeCooldown -= deltaTime;
		if (m_DirChangeCooldown <= 0.f)
		{
			m_PatrolDir = (rand() % 2 == 0) ? 1.f : -1.f;
			m_DirChangeCooldown = DIR_CHANGE_INTERVAL * (0.5f + static_cast<float>(rand() % 100) / 100.f);
			m_FlipCooldown = FLIP_COOLDOWN;
		}

		float separationDir = 0.f;
		const auto MY_POS = GetOwner()->GetWorldPosition();

		for (const auto& go : m_Scene.GetGameObjects())
		{
			if (!go || go.get() == GetOwner()) continue;
			auto* otherAI = go->GetComponent<EnemyAIComponent>();
			if (!otherAI || otherAI->m_TypeId != "ZenChan") continue;

			const float dx = MY_POS.x - go->GetWorldPosition().x;
			if (std::abs(dx) < SEPARATION_DIST)
				separationDir += (dx >= 0.f) ? 1.f : -1.f;
		}

		float finalDir = m_PatrolDir;
		if (std::abs(separationDir) > 0.f)
			finalDir = (separationDir > 0.f) ? 1.f : -1.f;

		pPhys->RequestMoveX(finalDir, MOVE_SPEED);

		if (auto* pTex = GetOwner()->GetComponent<TextureComponent>())
			pTex->FlipX = (finalDir < 0.f);

		m_JumpCooldown -= deltaTime;
		if (m_JumpCooldown <= 0.f && pPhys->IsGrounded())
		{
			pPhys->SetVelocityY(-500.f);
			m_JumpCooldown = JUMP_INTERVAL;
		}
	}

	void EnemyAIComponent::UpdateMaita(float deltaTime)
	{
		auto* pPhys = GetOwner()->GetComponent<PhysicsComponent>();
		if (!pPhys) return;

		const float dir = GetDirToNearestPlayer();
		pPhys->RequestMoveX(dir, MOVE_SPEED);

		// Only flip sprite if the player is far enough to avoid oscillation
		if (dir != 0.f)
		{
			const auto myPos = GetOwner()->GetWorldPosition();
			float nearestDx = FLT_MAX;
			for (const auto& go : m_Scene.GetGameObjects())
			{
				if (!go || !go->GetComponent<PlayerComponent>()) continue;
				const float dx = go->GetWorldPosition().x - myPos.x;
				if (std::abs(dx) < std::abs(nearestDx))
					nearestDx = dx;
			}

			if (std::abs(nearestDx) > FLIP_DEADZONE)
			{
				if (auto* pTex = GetOwner()->GetComponent<TextureComponent>())
					pTex->FlipX = (nearestDx < 0.f);
			}
		}

		m_ShootCooldown -= deltaTime;
		if (m_ShootCooldown <= 0.f)
		{
			const auto MY_POS = GetOwner()->GetWorldPosition();
			float nearestDist = FLT_MAX;
			float shootDir = dir != 0.f ? dir : 1.f;

			for (const auto& go : m_Scene.GetGameObjects())
			{
				if (!go || !go->GetComponent<PlayerComponent>()) continue;
				const float DX = go->GetWorldPosition().x - MY_POS.x;
				const float DIST = std::abs(DX);
				if (DIST < nearestDist)
				{
					nearestDist = DIST;
					shootDir = (DX >= 0.f) ? 1.f : -1.f;
				}
			}

			if (nearestDist < SHOOT_RANGE)
				ShootRock(shootDir);

			m_ShootCooldown = SHOOT_INTERVAL;
		}
	}

	float EnemyAIComponent::GetDirToNearestPlayer() const
	{
		const auto MY_POS = GetOwner()->GetWorldPosition();
		float bestDist = FLT_MAX;
		float bestDir = 0.f;

		for (const auto& go : m_Scene.GetGameObjects())
		{
			if (!go || !go->GetComponent<PlayerComponent>()) continue;
			const float DX = go->GetWorldPosition().x - MY_POS.x;
			const float DIST = std::abs(DX);
			if (DIST < bestDist)
			{
				bestDist = DIST;
				bestDir = (DX > 0.f) ? 1.f : -1.f;
			}
		}
		return bestDir;
	}

	void EnemyAIComponent::ShootRock(float dirX)
	{
		auto* pOwner = GetOwner();
		const auto POS = pOwner->GetWorldPosition();

		LevelCollisionComponent* pLevelCol = nullptr;
		for (const auto& go : m_Scene.GetGameObjects())
		{
			if (!go) continue;
			if (auto* col = go->GetComponent<LevelCollisionComponent>())
			{
				pLevelCol = col;
				break;
			}
		}

		constexpr float SIZE{ 20.f };
		const float SPAWN_X{ POS.x + dirX * (SIZE + 4.f) };
		const float SPAWN_Y{ POS.y - 30.f };

		auto pRock = std::make_unique<GameObject>();
		pRock->SetLocalPosition(SPAWN_X, SPAWN_Y);
		pRock->AddComponent<TextureComponent>(SIZE, SIZE)->SetTexture("Characters/Maita/MaitaRock.png");
		pRock->AddComponent<BoxColliderComponent>(SIZE, SIZE);
		//pRock->AddComponent<BoxColliderDebugDrawComponent>(255, 165, 0);
		pRock->AddComponent<MaitaRockProjectileComponent>(m_Scene, dirX, pLevelCol);

		m_Scene.Add(std::move(pRock));
	}
}