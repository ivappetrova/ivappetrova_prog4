#ifndef MAITA_BALLOON_PROJECTILE_COMPONENT_H
#define MAITA_BALLOON_PROJECTILE_COMPONENT_H

#include "Component.h"
#include "Scene.h"

namespace dae
{
	class LevelCollisionComponent;

	class MaitaRockProjectileComponent final : public Component
	{
	public:
		MaitaRockProjectileComponent(GameObject* owner, Scene& scene, float dirX, float windowHeight, LevelCollisionComponent* pLevelCol);
		void Update(float deltaTime) override;

	private:
		Scene& m_Scene;
		float  m_DirX;
		[[maybe_unused]] float m_WindowHeight;
		float  m_Speed{ 200.f };
		float  m_Lifetime{ 4.f };
		LevelCollisionComponent* m_pLevelCol;
	};
}
#endif