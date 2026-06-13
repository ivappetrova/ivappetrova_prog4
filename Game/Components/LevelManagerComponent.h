#ifndef LEVELMANAGERCOMPONENT_H
#define LEVELMANAGERCOMPONENT_H

#include "Component.h"
#include "Utils/LevelLoader.h"
#include <memory>
#include <vector>

namespace dae
{
	class Scene;
	class GameObject;
	class EnemyComponent;
	class BoxColliderComponent;
	class PickupComponent;

	class LevelManagerComponent final : public Component
	{
	public:
		LevelManagerComponent(
			GameObject* owner,
			Scene& scene,
			std::shared_ptr<LevelLoader>      loader,
			std::vector<GameObject*>          players,
			float                             windowWidth,
			float                             windowHeight);

		void Update(float deltaTime) override;
		void SkipLevel();
		void SetOnGameOver(std::function<void()> callback) { m_OnGameOver = std::move(callback); }

		// Rule of 5
		LevelManagerComponent(const LevelManagerComponent&) = delete;
		LevelManagerComponent& operator=(const LevelManagerComponent&) = delete;
		LevelManagerComponent(LevelManagerComponent&&) = delete;
		LevelManagerComponent& operator=(LevelManagerComponent&&) = delete;

	private:
		void LoadLevel(int idx);
		void SpawnExitPortal();

		Scene& m_Scene;
		std::shared_ptr<LevelLoader> m_pLoader;
		std::vector<GameObject*> m_Players;
		std::vector<GameObject*> m_Enemies;
		std::function<void()> m_OnGameOver;
		std::vector<GameObject*> m_LevelObjects;
		GameObject* m_pExitPortal{ nullptr };
		int m_CurrentLevel{ 0 };
		bool m_Transitioning{ false };
		bool m_PortalSpawned{ false };
		float m_WindowWidth;
		float m_WindowHeight;
	};

}

#endif