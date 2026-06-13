#ifndef BUBBLE_PROJECTILE_COMPONENT_H
#define BUBBLE_PROJECTILE_COMPONENT_H

#include "Component.h"
#include "GameObject.h"

namespace dae
{
	class BoxColliderComponent;
	class EnemyComponent;
	class Scene;
	class LevelCollisionComponent;

	class BubbleProjectileComponent final : public Component
	{
	public:
		BubbleProjectileComponent(GameObject* owner, Scene& scene, float directionX, GameObject* shooter, float windowHeight, LevelCollisionComponent* pLevelCol);

		void Update(float deltaTime) override;

		static constexpr float TRAVEL_SPEED{ 300.f };
		static constexpr float TRAVEL_TIME{ 1.4f };  // seconds before bubble "empties"
		static constexpr float TRAP_TIMEOUT{ 5.f };   // seconds until enemy escapes
		static constexpr float FLOAT_SPEED{ 40.f };   // upward drift while trapping
		static constexpr float EMPTY_LIFETIME{ 0.6f };  // how long an empty bubble lingers

	private:
		enum class Phase { Traveling, Trapped, Empty };

		void UpdateTraveling(float dt);
		void UpdateTrapped(float dt);
		void UpdateEmpty(float dt);

		EnemyComponent* FindOverlappingEnemy() const;
		bool AnyPlayerOverlaps() const;
		void BurstBubble();
		void ReleaseTrappedEnemy();

		Phase m_Phase{ Phase::Traveling };
		float m_Timer{ TRAVEL_TIME };
		float m_DirX{ 1.f };

		Scene& m_Scene;
		float m_WindowHeight;
		GameObject* m_pShooter{ nullptr }; 
		GameObject* m_pTrappedEnemy{ nullptr };

		BoxColliderComponent* m_pCollider{ nullptr };
		LevelCollisionComponent* m_pLevelCol{ nullptr };
	};
}
#endif