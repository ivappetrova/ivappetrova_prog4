#pragma once
#include "GameObjectCommand.h"
#include "GameObject.h"
#include "PlayerPointsComponent.h"

namespace dae
{
	class PickUpCommand final : public GameObjectCommand
	{
	public:
		PickUpCommand(GameObject* gameObject, int points)
			: GameObjectCommand(gameObject), m_Points(points)
		{
		}

		void Execute() override
		{
			if (auto* points = GetGameObject()->GetComponent<PlayerPointsComponent>())
				points->AddPoints(m_Points);
		}

	private:
		int m_Points;
	};
}