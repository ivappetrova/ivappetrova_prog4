#ifndef PICKUP_COMPONENT_H
#define PICKUP_COMPONENT_H

#include "Component.h"
#include "GameObject.h"
#include "Scene.h"

namespace dae
{
	class BoxColliderComponent;

	class PickUpComponent final : public Component
	{
	public:
		PickUpComponent(GameObject* owner, Scene& scene, int points);

		void Update(float deltaTime) override;
		bool IsReady() const { return m_SpawnDelay <= 0.f; }

		int GetPoints() const { return m_Points; }

		// Rule of 5
		PickUpComponent(const PickUpComponent&) = delete;
		PickUpComponent& operator=(const PickUpComponent&) = delete;
		PickUpComponent(PickUpComponent&&) = delete;
		PickUpComponent& operator=(PickUpComponent&&) = delete;

	private:
		Scene& m_Scene;
		int m_Points;
		bool m_Collected{ false };

		BoxColliderComponent* m_pCollider{ nullptr };

		float m_SpawnDelay{ 0.3f };
	};
}
#endif