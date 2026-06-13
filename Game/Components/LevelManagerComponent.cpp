#include "LevelManagerComponent.h"
#include "Utils/LevelLoader.h"
#include "EnemyComponent.h"
#include "Components/BoxColliderComponent.h"
#include "Components/BoxColliderDebugDrawComponent.h"
#include "Components/PickupComponent.h"
#include "Scene.h"
#include "GameObject.h"
#include <iostream>
#include <algorithm>
#include <memory>

namespace dae
{

	LevelManagerComponent::LevelManagerComponent(
		GameObject* owner,
		Scene& scene,
		std::shared_ptr<LevelLoader> loader,
		std::vector<GameObject*>     players,
		float                        windowWidth,
		float                        windowHeight)
		: Component(owner)
		, m_Scene{ scene }
		, m_pLoader{ std::move(loader) }
		, m_Players{ std::move(players) }
		, m_WindowWidth{ windowWidth }
		, m_WindowHeight{ windowHeight }
	{
		LoadLevel(0);
	}

	void LevelManagerComponent::Update(float /*dt*/)
	{
		if (m_Transitioning) return;

		// ── Step 1: wait for all enemies to die, then show the portal ────────
		if (!m_PortalSpawned)
		{
			// Erase enemies that are no longer alive BEFORE the scene frees them.
			// IsAlive() is set to false by Kill() before MarkForDestroy(), so the
			// pointer is still valid here. Once erased we never dereference it again,
			// avoiding the dangling-pointer crash that hit GetComponent<EnemyComponent>().
			m_Enemies.erase(
				std::remove_if(m_Enemies.begin(), m_Enemies.end(),
					[](GameObject* pE)
					{
						if (!pE) return true;
						const auto* ec = pE->GetComponent<EnemyComponent>();
						return !ec || !ec->IsAlive();
					}),
				m_Enemies.end());

			if (m_Enemies.empty())
			{
				std::cout << "[LevelManager] All enemies dead — exit portal opened!\n";
				SpawnExitPortal();
			}
			return;
		}

		// ── Step 2: portal is visible; check if any player touches it ────────
		if (!m_pExitPortal) return;

		auto* pPortalCol = m_pExitPortal->GetComponent<BoxColliderComponent>();
		if (!pPortalCol) return;

		for (auto* pPlayer : m_Players)
		{
			if (!pPlayer) continue;
			auto* pPlayerCol = pPlayer->GetComponent<BoxColliderComponent>();
			if (pPlayerCol && pPlayerCol->Overlaps(*pPortalCol))
			{
				m_Transitioning = true;
				std::cout << "[LevelManager] Player entered portal — loading level "
					<< m_CurrentLevel + 1 << ".\n";
				LoadLevel(m_CurrentLevel + 1);
				return;
			}
		}
	}

	void LevelManagerComponent::SkipLevel()
	{
		LoadLevel(m_CurrentLevel + 1);
	}

	void LevelManagerComponent::SpawnExitPortal()
	{
		const float PORTAL_SIZE{ 60.f };
		const float PORTAL_X{ 100.f };
		const float PORTAL_Y{ m_WindowHeight - 50.f };

		auto portalGO = std::make_unique<GameObject>();
		portalGO->SetLocalPosition(PORTAL_X, PORTAL_Y);
		portalGO->AddComponent<BoxColliderComponent>(PORTAL_SIZE, PORTAL_SIZE);
		portalGO->AddComponent<BoxColliderDebugDrawComponent>(0, 255, 80);

		m_pExitPortal = portalGO.get();
		m_LevelObjects.push_back(portalGO.get());
		m_Scene.Add(std::move(portalGO));

		m_PortalSpawned = true;
	}

	void LevelManagerComponent::LoadLevel(int idx)
	{
		if (idx >= m_pLoader->GetLevelCount())
		{
			std::cout << "[LevelManager] All levels complete!\n";
			return;
		}

		// Remove any dynamically-spawned pickups (food dropped by bubbles).
		// These are never added to m_LevelObjects so we scan the scene for them.
		for (const auto& pGO : m_Scene.GetGameObjects())
		{
			if (pGO && pGO->GetComponent<PickupComponent>())
				m_Scene.Remove(*pGO);
		}

		// Remove all GameObjects that belonged to the previous level
		// (background, collision geometry, enemies, portal).
		for (auto* pObj : m_LevelObjects)
			m_Scene.Remove(*pObj);
		m_LevelObjects.clear();
		m_Enemies.clear();
		m_pExitPortal = nullptr;
		m_PortalSpawned = false;

		m_CurrentLevel = idx;
		m_Transitioning = false;

		auto result = m_pLoader->LoadLevel(
			m_Scene, idx,
			m_WindowWidth, m_WindowHeight,
			m_Players);

		m_Enemies = std::move(result.enemies);
		m_LevelObjects = std::move(result.levelObjects);
	}

}