#ifndef PICKUPCOMMAND_H
#define PICKUPCOMMAND_H

#include "GameObjectCommand.h"
#include "GameObject.h"
#include "Components/ScoreComponent.h"

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
			if (auto* pScore = GetGameObject()->GetComponent<ScoreComponent>())
			{
				pScore->AddPoints(m_Points);
			}
		}

	private:
		int m_Points;
	};
}
#endif