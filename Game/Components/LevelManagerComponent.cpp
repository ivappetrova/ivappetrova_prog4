#include "LevelManagerComponent.h"
#include "LevelLoader.h"
#include "EnemyComponent.h"
#include "Components/BoxColliderComponent.h"
#include "Scene.h"
#include "GameObject.h"
#include <iostream>
#include <algorithm>

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

		const bool allDead = !m_Enemies.empty() &&
			std::all_of(m_Enemies.begin(), m_Enemies.end(),
				[](GameObject* pE)
				{
					auto* ec = pE ? pE->GetComponent<EnemyComponent>() : nullptr;
					return !ec || !ec->IsAlive();
				});

		if (allDead)
		{
			m_Transitioning = true;
			std::cout << "[LevelManager] Level " << m_CurrentLevel
				<< " cleared! Loading level " << m_CurrentLevel + 1 << ".\n";
			LoadLevel(m_CurrentLevel + 1);
		}
	}


	void LevelManagerComponent::LoadLevel(int idx)
	{
		if (idx >= m_pLoader->GetLevelCount())
		{
			std::cout << "[LevelManager] All levels complete!\n";
			return;
		}

		// Remove all GameObjects that belonged to the previous level
		for (auto* pObj : m_LevelObjects)
			m_Scene.Remove(*pObj);
		m_LevelObjects.clear();
		m_Enemies.clear();

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