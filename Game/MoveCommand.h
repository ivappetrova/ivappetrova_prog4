#pragma once
#include "Command.h"
#include "GameObject.h"
#include "DeltaTime.h"

namespace dae
{
	class MoveCommand final : public Command
	{
	public:
		MoveCommand(GameObject* gameObject, const float dirX, const float dirY, const float speed)
			: m_pGameObject{ gameObject }, m_DIR_X(dirX), m_DIR_Y{ dirY }, m_SPEED{ speed }
		{
		}

		void Execute() override
		{
			const float DELTA_TIME = DeltaTime::Get();
			const auto& POS = m_pGameObject->GetWorldPosition();
			m_pGameObject->SetLocalPosition(POS.x + m_DIR_X * m_SPEED * DELTA_TIME, POS.y + m_DIR_Y * m_SPEED * DELTA_TIME);
		}

	private:
		GameObject* m_pGameObject;
		float m_DIR_X;
		float m_DIR_Y;
		float m_SPEED;
	};
}