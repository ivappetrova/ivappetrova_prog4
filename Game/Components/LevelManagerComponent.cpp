#include "LevelManagerComponent.h"
#include "Utils/LevelLoader.h"
#include "EnemyComponent.h"
#include "Components/BoxColliderComponent.h"
#include "Components/BoxColliderDebugDrawComponent.h"
#include "Components/PickUpComponent.h"
#include "Components/HealthComponent.h"
#include "Scene.h"
#include "GameObject.h"
#include <iostream>
#include <algorithm>
#include <memory>

namespace dae
{

	LevelManagerComponent::LevelManagerComponent( GameObject* owner, Scene& scene, std::shared_ptr<LevelLoader> loader, std::vector<GameObject*>players, float windowWidth, float windowHeight)
		: Component(owner), m_Scene{ scene }, m_pLoader{ std::move(loader) }, m_Players{ std::move(players) }, m_WindowWidth{ windowWidth }, m_WindowHeight{ windowHeight }
	{
		LoadLevel(0);
	}

	void LevelManagerComponent::Update(float /*dt*/)
	{
		bool allPlayersDead = std::all_of(m_Players.begin(), m_Players.end(),
			[](GameObject* pP)
			{
				if (!pP) return true;
				auto* pH = pP->GetComponent<HealthComponent>();
				return !pH || pH->GetHP() <= 0;
			});

		if (allPlayersDead)
		{
			if (m_OnGameOver) m_OnGameOver();
			return;
		}

		if (m_Transitioning) return;

		if (!m_PortalSpawned)
		{
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
				std::cout << "[LevelManager] All enemies dead — exit portal opened!"<< std::endl;
				SpawnExitPortal();
			}
			return;
		}

		// Portal is visible; check if any player touches it
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
				std::cout << "[LevelManager] Player entered portal — loading level " << m_CurrentLevel + 1 << "." << std::endl;
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
			if (m_OnGameOver) m_OnGameOver();
			return;
		}

		for (const auto& pGO : m_Scene.GetGameObjects())
		{
			if (pGO && pGO->GetComponent<PickupComponent>())
			{
				m_Scene.Remove(*pGO);
			}
		}

		// Remove all GameObjects that belonged to the previous level
		for (auto* pObj : m_LevelObjects)
		{
			m_Scene.Remove(*pObj);
		}
		m_LevelObjects.clear();
		m_Enemies.clear();
		m_pExitPortal = nullptr;
		m_PortalSpawned = false;

		m_CurrentLevel = idx;
		m_Transitioning = false;

		auto result = m_pLoader->LoadLevel( m_Scene, idx, m_WindowWidth, m_WindowHeight, m_Players);

		m_Enemies = std::move(result.enemies);
		m_LevelObjects = std::move(result.levelObjects);
	}

}