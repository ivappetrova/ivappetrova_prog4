#pragma once
#include "Command.h"
#include "GameObject.h"
#include "DeltaTime.h"

namespace dae
{
	class MoveCommand final : public Command
	{
	public:
		MoveCommand(GameObject* gameObject, float dirX, float dirY, float speed)
			: m_pGameObject{ gameObject }, m_DirX(dirX), m_DirY{ dirY }, m_Speed{ speed }
		{
		}

		void Execute() override
		{
			const float DELTA_TIME = DeltaTime::Get();
			const auto& POS = m_pGameObject->GetWorldPosition();
			m_pGameObject->SetLocalPosition(POS.x + m_DirX * m_Speed* DELTA_TIME, POS.y + m_DirY * m_Speed* DELTA_TIME);
		}

	private:
		GameObject* m_pGameObject;
		float m_DirX;
		float m_DirY;
		float m_Speed;

	};
}