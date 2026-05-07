#ifndef MOVECOMMAND_H
#define MOVECOMMAND_H

#include "Command.h"
#include "GameObject.h"
#include "Player.h"

namespace dae
{
	class MoveCommand final : public Command
	{
	public:
		MoveCommand(GameObject* gameObject, float dirX)
			: m_pGameObject(gameObject), m_DirX(dirX)
		{
		}

		void Execute() override
		{
			if (auto* player = m_pGameObject->GetComponent<Player>())
			{
				player->SetMoveInput(m_DirX);
			}
		}

	private:
		GameObject* m_pGameObject;
		float m_DirX;
	};
}
#endif