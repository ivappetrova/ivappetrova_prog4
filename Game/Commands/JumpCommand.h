#ifndef JUMPCOMMAND_H
#define JUMPCOMMAND_H

#include "Command.h"
#include "GameObject.h"
#include "Components/PlayerComponent.h"

namespace dae
{
	class JumpCommand final : public Command
	{
	public:
		explicit JumpCommand(GameObject* gameObject)
			: m_pGameObject(gameObject)
		{
		}

		void Execute() override
		{
			if (auto* pPlayer = m_pGameObject->GetComponent<PlayerComponent>())
			{
				pPlayer->SetJumpInput(true);
			}
		}

	private:
		GameObject* m_pGameObject;
	};
}
#endif