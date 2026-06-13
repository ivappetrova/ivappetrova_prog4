#ifndef SHOOT_COMMAND_H
#define SHOOT_COMMAND_H

#include "GameObjectCommand.h"
#include "Components/PlayerComponent.h"
#include "States/ShootState.h"
#include "Scene.h"

namespace dae
{
	class ShootCommand final : public GameObjectCommand
	{
	public:
		ShootCommand(GameObject* playerGO, Scene& scene, sound_id shootSound, bool pvp = false)
			: GameObjectCommand(playerGO), m_Scene(scene), m_ShootSound(shootSound), m_PvP(pvp)
		{
		}

		void Execute() override
		{
			auto* pPlayer = GetGameObject()->GetComponent<PlayerComponent>();
			if (!pPlayer) return;
			if (std::string(pPlayer->GetCurrentStateName()) == "Shoot") return;
			const bool WAS_MOVING = std::string(pPlayer->GetCurrentStateName()) == "Move";
			pPlayer->ChangeState(new ShootState{ m_Scene, WAS_MOVING, m_ShootSound, m_PvP });
		}

	private:
		Scene& m_Scene;
		sound_id m_ShootSound{};
		bool m_PvP{ false };
	};
}
#endif