#ifndef SKIPLEVELCOMMAND_H
#define SKIPLEVELCOMMAND_H

#include "Command.h"
#include "GameObject.h"
#include "Components/LevelManagerComponent.h"

namespace dae
{
	class SkipLevelCommand final : public Command
	{
	public:
		explicit SkipLevelCommand(GameObject* levelManager)
			: m_pLevelManager(levelManager)
		{
		}

		void Execute() override
		{
			if (auto* pLM = m_pLevelManager->GetComponent<LevelManagerComponent>())
				pLM->SkipLevel();
		}

	private:
		GameObject* m_pLevelManager;
	};
}
#endif