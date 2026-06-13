#ifndef PVP_BUBBLE_PROJECTILE_COMPONENT_H
#define PVP_BUBBLE_PROJECTILE_COMPONENT_H

#include "Component.h"

namespace dae
{
	class BoxColliderComponent;
	class Scene;

	class PvPBubbleProjectileComponent final : public Component
	{
	public:
		PvPBubbleProjectileComponent(GameObject* owner, Scene& scene, float directionX, GameObject* shooter);

		void Update(float deltaTime) override;

		static constexpr float TRAVEL_SPEED{ 300.f };
		static constexpr float TRAVEL_TIME{ 1.4f };

	private:
		void UpdateTraveling(float dt);

		Scene& m_Scene;
		float m_DirX;
		GameObject* m_pShooter{ nullptr };
		float m_Timer{ TRAVEL_TIME };

		BoxColliderComponent* m_pCollider{ nullptr };
	};
}
#endif