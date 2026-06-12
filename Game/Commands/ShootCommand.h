#pragma once
#ifndef SHOOT_COMMAND_H
#define SHOOT_COMMAND_H

// ============================================================================
//  ShootCommand
//
//  Bind this to a key/button to make a player enter ShootState.
//  The ShootState constructor needs a Scene& — we pass it here.
//
//  Usage (in load()):
//    input.BindKeyboardCommand(
//        SDL_SCANCODE_SPACE,
//        InputManager::KeyState::Down,
//        std::make_unique<ShootCommand>(pChar1, scene));
// ============================================================================

#include "GameObjectCommand.h"
#include "Components/PlayerComponent.h"
#include "States/ShootState.h"
#include "Scene.h"

namespace dae
{
	class ShootCommand final : public GameObjectCommand
	{
	public:
		ShootCommand(GameObject* playerGO, Scene& scene)
			: GameObjectCommand(playerGO)
			, m_Scene(scene)
		{}

		void Execute() override
		{
			auto* pPlayer = GetGameObject()->GetComponent<PlayerComponent>();
			if (!pPlayer) return;

			// Avoid stacking shoot states
			if (std::string(pPlayer->GetCurrentStateName()) == "Shoot") return;

			const bool wasMoving =
				std::string(pPlayer->GetCurrentStateName()) == "Move";

			pPlayer->ChangeState(new ShootState{ m_Scene, wasMoving });
		}

	private:
		Scene& m_Scene;
	};

} // namespace dae

#endif // SHOOT_COMMAND_H