#include "ShootState.h"
#include "IdleState.h"
#include "MoveState.h"

#include "Components/PlayerComponent.h"
#include "Components/BoxColliderComponent.h"
#include "Components/BubbleProjectileComponent.h"
#include "Components/BoxColliderDebugDrawComponent.h"
#include "Components/PhysicsComponent.h"
#include "Components/LevelCollisionComponent.h"

#include "Scene.h"
#include "GameObject.h"
#include "TextureComponent.h"

#include <iostream>
#include <memory>
#include "ServiceLocator.h"

namespace dae
{
	ShootState::ShootState(Scene& scene, bool wasMoving, sound_id shootSound)
		: m_Scene(scene), m_WasMoving(wasMoving), m_ShootSound(shootSound)
	{
	}

	void ShootState::Enter(PlayerComponent& player)
	{
		std::cout << "[ShootState] Entered" << std::endl;
		m_Timer = SHOOT_DURATION;
		player.StopMove();
		ServiceLocator::GetSoundSystem().Play(m_ShootSound, 0.8f);

		GameObject* pPlayerGO = player.GetGameObject();

		auto* pTex = pPlayerGO->GetComponent<TextureComponent>();
		if (!pTex) return;

		float DIR_X{ pTex->FlipX ? 1.f : -1.f };
		const float PLAYER_HEIGHT { pTex->GetHeight() };
		float windowHeight{};
		if (auto* pPhys = pPlayerGO->GetComponent<PhysicsComponent>())
		{
			windowHeight = pPhys->GetWindowHeight();
		}

		const auto PLAYER_POS = pPlayerGO->GetWorldPosition();
		const float BUBBLE_WIDTH{ 25.f };
		const float BUBBLE_HEIGHT{ 25.f };

		const float OFFSET_X{ DIR_X * (BUBBLE_WIDTH + 4.f) };
		const float SPAWN_X{ PLAYER_POS.x + OFFSET_X };
		const float SPAWN_Y{ PLAYER_POS.y - PLAYER_HEIGHT * 0.5f };

		auto pBubbleGameObj = std::make_unique<GameObject>();
		pBubbleGameObj->SetLocalPosition(SPAWN_X, SPAWN_Y);
		pBubbleGameObj->AddComponent<TextureComponent>(BUBBLE_WIDTH, BUBBLE_HEIGHT)->SetTexture("Characters/Player/BubbleProjectile.png");
		pBubbleGameObj->AddComponent<BoxColliderComponent>(BUBBLE_WIDTH, BUBBLE_HEIGHT);
		pBubbleGameObj->AddComponent<BoxColliderDebugDrawComponent>(255, 0, 255);
		
		LevelCollisionComponent* pLevelCol{ nullptr };
		for (const auto& go : m_Scene.GetGameObjects())
		{
			if (!go) continue;
			if (auto* col = go->GetComponent<LevelCollisionComponent>())
			{
				pLevelCol = col;
				break;
			}
		}

		pBubbleGameObj->AddComponent<BubbleProjectileComponent>(m_Scene, DIR_X, pPlayerGO, windowHeight, pLevelCol);

		m_Scene.Add(std::move(pBubbleGameObj));
	}

	PlayerState* ShootState::HandleInput(PlayerComponent& player)
	{
		if (m_Timer <= 0.f)
		{
			if (m_WasMoving && player.GetMoveDirX() != 0.f)
			{
				return new MoveState{};
			}
			return new IdleState{};
		}
		return nullptr;
	}

	void ShootState::Update(PlayerComponent& /*player*/, float deltaTime)
	{
		m_Timer -= deltaTime;
	}

}