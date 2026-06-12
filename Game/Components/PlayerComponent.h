#ifndef PLAYER_H
#define PLAYER_H

#include "Component.h"
#include "IObserver.h"
#include "Event.h"
#include "Scene.h"

namespace dae
{
	class PlayerState;
	class PhysicsComponent;

	class PlayerComponent final : public Component, public IObserver
	{
	public:
		explicit PlayerComponent(GameObject* owner, float movementSpeed, Scene& scene);
		~PlayerComponent() override;

		void Update(float deltaTime) override;
		void Notify(const Event& event, GameObject* actor) override;

		// Input — called by commands each frame
		void SetMoveInput(float dirX);
		void SetJumpInput(bool wants);
		void SetShootInput(bool wants);

		// Read by states to decide transitions
		float GetMoveDirX() const { return m_MoveDirX; }
		bool  WantsJump()   const { return m_WantsJump; }
		bool  WantsShoot()  const { return m_WantsShoot; }
		bool  IsGrounded()  const;
		float GetVelocityY() const;

		// Physics actions— states call these, never PhysicsComponent directly
		void RequestMove(float dirX);
		void StopMove();
		void Jump(float jumpSpeed = 550.f);
		void ApplyKnockback(float vx);
		void StopHorizontal();
		void DecayHorizontalVelocity(float deltaTime, float drag = 0.05f);

		GameObject* GetGameObject() const { return GetOwner(); }
		float GetMoveSpeed() const { return m_MoveSpeed; }
		Scene& GetScene() const { return m_Scene; }

		// State machine
		void ChangeState(PlayerState* newState);
		const char* GetCurrentStateName() const;

		// Rule of 5
		PlayerComponent(const PlayerComponent&) = delete;
		PlayerComponent(PlayerComponent&&) = delete;
		PlayerComponent& operator=(const PlayerComponent&) = delete;
		PlayerComponent& operator=(PlayerComponent&&) = delete;

	private:
		void InitObserver();

		PlayerState* m_pCurrentState{ nullptr };
		PhysicsComponent* m_pPhysics{ nullptr };

		float m_MoveDirX{ 0.f };
		bool  m_WantsJump{ false };
		bool  m_WantsShoot{ false };

		float m_MoveSpeed;
		Scene& m_Scene;
	};
}
#endif