#pragma once
#include "Component.h"
#include "IObserver.h"
#include "Event.h"

namespace dae
{
	class PlayerState;

	class Player final : public Component, public IObserver
	{
	public:
		explicit Player(GameObject* owner);
		~Player() override;

		void Update(float deltaTime) override;
		void Notify(const Event& event, GameObject* actor) override;

		// Called by MoveCommand each frame
		void SetMoveInput(float dirX);
		void SetJumpInput(bool wants);
		void SetShootInput(bool wants);

		// Called by states to move the character
		void SetVelocity(float vx, float vy);
		void AddVelocity(float ax, float ay);
		float GetVelocityX() const { return m_VelocityX; }
		float GetVelocityY() const { return m_VelocityY; }

		bool  IsGrounded()    const { return m_IsGrounded; }
		void  SetGrounded(bool g) { m_IsGrounded = g; }

		float GetMoveDirX()  const { return m_MoveDirX; }
		bool  WantsJump()    const { return m_WantsJump; }
		bool  WantsShoot()   const { return m_WantsShoot; }

		GameObject* GetGameObject() const { return GetOwner(); }

		// State machine
		void ChangeState(PlayerState* newState);
		const char* GetCurrentStateName() const;


		// Rule of 5
		Player(const Player&) = delete;
		Player(Player&&) = delete;
		Player& operator=(const Player&) = delete;
		Player& operator=(Player&&) = delete;

	private:
		void InitObserver();
		void ClearFrameInput();

		PlayerState* m_pCurrentState{ nullptr };

		float m_MoveDirX{ 0.f };
		bool  m_WantsJump{ false };
		bool  m_WantsShoot{ false };

		float m_VelocityX{ 0.f };
		float m_VelocityY{ 0.f };
		bool  m_IsGrounded{ true };
	};

}