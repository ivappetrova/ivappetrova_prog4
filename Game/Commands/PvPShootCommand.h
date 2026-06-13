#ifndef PVP_SHOOT_COMMAND_H
#define PVP_SHOOT_COMMAND_H

#include "GameObjectCommand.h"
#include "Components/PlayerComponent.h"
#include "States/PvPShootState.h"
#include "Scene.h"

namespace dae
{
	class PvPShootCommand final : public GameObjectCommand
	{
	public:
		PvPShootCommand(GameObject* playerGO, Scene& scene, sound_id shootSound)
			: GameObjectCommand(playerGO), m_Scene(scene), m_ShootSound(shootSound)
		{}

		void Execute() override
		{
			auto* pPlayer = GetGameObject()->GetComponent<PlayerComponent>();
			if (!pPlayer) return;
			if (std::string(pPlayer->GetCurrentStateName()) == "Shoot") return;
			const bool WAS_MOVING = std::string(pPlayer->GetCurrentStateName()) == "Move";
			pPlayer->ChangeState(new PvPShootState{ m_Scene, WAS_MOVING, m_ShootSound });
		}

	private:
		Scene& m_Scene;
		sound_id m_ShootSound{};
	};
}
#endif