#pragma once
#ifndef PICKUP_COMPONENT_H
#define PICKUP_COMPONENT_H

#include "Component.h"
#include "GameObject.h"
#include "Scene.h"

namespace dae
{
	class BoxColliderComponent;

	class PickupComponent final : public Component
	{
	public:
		PickupComponent(GameObject* owner, Scene& scene, int points);

		void Update(float deltaTime) override;
		bool IsReady() const { return m_SpawnDelay <= 0.f; }

		int GetPoints() const { return m_Points; }

		// Rule of 5
		PickupComponent(const PickupComponent&) = delete;
		PickupComponent& operator=(const PickupComponent&) = delete;
		PickupComponent(PickupComponent&&) = delete;
		PickupComponent& operator=(PickupComponent&&) = delete;

	private:
		Scene& m_Scene;
		int m_Points;
		bool m_Collected{ false };

		BoxColliderComponent* m_pCollider{ nullptr };

		float m_SpawnDelay{ 0.3f };
	};

}

#endif