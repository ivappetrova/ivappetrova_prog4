#include "ShootState.h"
#include "IdleState.h"
#include "MoveState.h"

#include "Components/PlayerComponent.h"
#include "Components/BoxColliderComponent.h"
#include "Components/BubbleProjectileComponent.h"
#include "Components/PvPBubbleProjectileComponent.h"
#include "Components/BoxColliderDebugDrawComponent.h"
#include "Components/PhysicsComponent.h"
#include "Components/LevelCollisionComponent.h"

#include "Scene.h"
#include "GameObject.h"
#include "TextureComponent.h"
#include "ServiceLocator.h"

namespace dae
{
	ShootState::ShootState(Scene& scene, bool wasMoving, sound_id shootSound, bool pvp)
		: m_Scene(scene), m_WasMoving(wasMoving), m_ShootSound(shootSound), m_PvP(pvp)
	{
	}

	void ShootState::Enter(PlayerComponent& player)
	{
		m_Timer = SHOOT_DURATION;
		player.StopMove();
		ServiceLocator::GetSoundSystem().Play(m_ShootSound, 0.8f);

		GameObject* pPlayerGO = player.GetGameObject();

		auto* pTex = pPlayerGO->GetComponent<TextureComponent>();
		if (!pTex) return;

		const float DIR_X = pTex->FlipX ? 1.f : -1.f;
		const float PLAYER_HEIGHT = pTex->GetHeight();
		const float BUBBLE_SIZE = 25.f;
		const auto  PLAYER_POS = pPlayerGO->GetWorldPosition();
		const float SPAWN_X = PLAYER_POS.x + DIR_X * (BUBBLE_SIZE + 4.f);
		const float SPAWN_Y = PLAYER_POS.y - PLAYER_HEIGHT * 0.5f;

		auto pBubble = std::make_unique<GameObject>();
		pBubble->SetLocalPosition(SPAWN_X, SPAWN_Y);
		pBubble->AddComponent<TextureComponent>(BUBBLE_SIZE, BUBBLE_SIZE)
			->SetTexture("Characters/Player/BubbleProjectile.png");
		pBubble->AddComponent<BoxColliderComponent>(BUBBLE_SIZE, BUBBLE_SIZE);
		pBubble->AddComponent<BoxColliderDebugDrawComponent>(255, 0, 255);

		if (m_PvP)
		{
			pBubble->AddComponent<PvPBubbleProjectileComponent>(m_Scene, DIR_X, pPlayerGO);
		}
		else
		{
			float windowHeight{};
			if (auto* pPhys = pPlayerGO->GetComponent<PhysicsComponent>())
				windowHeight = pPhys->GetWindowHeight();

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

			pBubble->AddComponent<BubbleProjectileComponent>(m_Scene, DIR_X, windowHeight, pLevelCol);
		}

		m_Scene.Add(std::move(pBubble));
	}

	PlayerState* ShootState::HandleInput(PlayerComponent& player)
	{
		if (m_Timer <= 0.f)
		{
			if (m_WasMoving && player.GetMoveDirX() != 0.f)
				return new MoveState{};
			return new IdleState{};
		}
		return nullptr;
	}

	void ShootState::Update(PlayerComponent& /*player*/, float deltaTime)
	{
		m_Timer -= deltaTime;
	}
}