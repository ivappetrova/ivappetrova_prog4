#pragma once
#include "Command.h"
#include "GameObject.h"

namespace dae
{
	class MoveCommand final : public Command
	{
	public:
		MoveCommand(GameObject* gameObject, float dx, float dy)
			: m_pGameObject(gameObject), m_Dx(dx), m_Dy(dy) {
		}

		void Execute() override
		{
			const auto& pos = m_pGameObject->GetWorldPosition();
			m_pGameObject->SetLocalPosition(pos.x + m_Dx, pos.y + m_Dy);
		}

	private:
		GameObject* m_pGameObject;
		float m_Dx;
		float m_Dy;
	};
}