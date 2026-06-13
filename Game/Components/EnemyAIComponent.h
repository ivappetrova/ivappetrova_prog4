#ifndef ENEMY_AI_COMPONENT_H
#define ENEMY_AI_COMPONENT_H

#include "Component.h"
#include "Scene.h"
#include <string>

namespace dae
{
	class LevelCollisionComponent;

	class EnemyAIComponent final : public Component
	{
	public:
		EnemyAIComponent(GameObject* owner, Scene& scene, const std::string& typeId, float windowHeight);
		void Update(float deltaTime) override;

	private:
		void UpdateZenChan(float deltaTime);
		void UpdateMaita(float deltaTime);
		void ShootRock(float dirX);
		float GetDirToNearestPlayer() const;

		Scene& m_Scene;
		std::string m_TypeId;

		float m_JumpCooldown{ 0.f };
		float m_ShootCooldown{ 0.f };
		float m_PatrolDir{ 1.f };
		float m_DirChangeCooldown{ 0.f };

		static constexpr float MOVE_SPEED{ 80.f };
		static constexpr float JUMP_INTERVAL{ 2.5f };
		static constexpr float SHOOT_INTERVAL{ 2.0f };
		static constexpr float SHOOT_RANGE{ 400.f };
		static constexpr float SEPARATION_DIST{ 70.f };
		static constexpr float DIR_CHANGE_INTERVAL{ 3.0f };

		float m_FlipCooldown{ 0.f };
		static constexpr float FLIP_COOLDOWN{ 0.5f };
		static constexpr float FLIP_DEADZONE{ 20.f };

		float m_LastPosX;
	};
}
#endif