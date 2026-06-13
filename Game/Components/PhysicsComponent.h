#ifndef PHYSICSCOMPONENT_H
#define PHYSICSCOMPONENT_H

#include "Component.h"
#include <glm/vec2.hpp>

namespace dae
{
	class LevelCollisionComponent;
	class BoxColliderComponent;

	class PhysicsComponent final : public Component
	{
	public:
		explicit PhysicsComponent(GameObject* owner, float screenHeight);

		void Update(float deltaTime) override;

		// Movement request - used from input commands, Physics applies the request only if the wall allows it
		void RequestMoveX(float direction, float speed);
		void ClearMoveX() { m_MoveRequest = 0.f; }

		// For jumps, knockback
		void SetVelocity(float vx, float vy) { m_Velocity = { vx, vy }; }
		void SetVelocityX(float vx) { m_Velocity.x = vx; }
		void SetVelocityY(float vy) { m_Velocity.y = vy; }
		void AddVelocity(float ax, float ay) { m_Velocity.x += ax; m_Velocity.y += ay; }
		glm::vec2 GetVelocity() const { return m_Velocity; }
		float GetWindowHeight() const { return m_WindowHeight; }

		// Grounded state— read by PlayerComponent states
		bool IsGrounded() const { return m_IsGrounded; }

		void SetLevelCollision(LevelCollisionComponent* levelCollider) { m_pLevelCollision = levelCollider; }

		// Rule of 5
		PhysicsComponent(const PhysicsComponent&) = delete;
		PhysicsComponent(PhysicsComponent&&) = delete;
		PhysicsComponent& operator=(const PhysicsComponent&) = delete;
		PhysicsComponent& operator=(PhysicsComponent&&) = delete;

	private:
		void ResolveCollisions(float deltaTime);

		float m_WindowHeight;

		glm::vec2 m_Velocity{};
		bool m_IsGrounded{ false };
		// Horizontal move request set by input commands each frame
		float m_MoveRequest{};  // -1, 0, or +1

		// Dirty flag
		float m_BlockedDirX{};

		// Shared for all instances
		static constexpr float m_GRAVITY{ 980.f };
		static constexpr float m_MAX_FALL_SPEED{ 900.f };

		float m_MoveSpeed{};

		LevelCollisionComponent* m_pLevelCollision{ nullptr };
	};
}
#endif