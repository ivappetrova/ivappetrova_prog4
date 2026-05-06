#pragma once
#include "PlayerState.h"
#include "Player.h"
#include "GameObject.h"
#include <iostream>

namespace dae
{

	//////////////////////////////////////////////////////////////// for HandleInput()
	class IdleState;
	class MoveState;
	class JumpState;
	class FallState;
	class ShootState;
	class TakeDmgState;

	//////////////////////////////////////////////////////////////// IdleState = used
	class IdleState final : public PlayerState
	{
	public:
		void Enter(Player& /*player*/) override
		{
			std::cout << "Entered IdleState" << std::endl;
		}

		PlayerState* HandleInput(Player& player) override;

		void Update(Player& /*player*/, float /*deltaTime*/) override {}
	};

	//////////////////////////////////////////////////////////////// MoveState = used
	class MoveState final : public PlayerState
	{
	public:
		void Enter(Player& /*player*/) override
		{
			std::cout << "Entered MoveState" << std::endl;
		}

		PlayerState* HandleInput(Player& player) override;

		void Update(Player& player, float deltaTime) override
		{
			player.SetVelocity(player.GetMoveDirX() * WALK_SPEED, 0.f);

			GameObject* gameObj = player.GetGameObject();
			const auto& POS = gameObj->GetWorldPosition();
			gameObj->SetLocalPosition(POS.x + player.GetVelocityX() * deltaTime, POS.y);
		}

	private:
		const float WALK_SPEED{80.f};
	};

	//////////////////////////////////////////////////////////////// JumpState = TODO
	class JumpState final : public PlayerState
	{
	public:
		void Enter(Player& /*player*/) override
		{
			std::cout << "Entered JumpState" << std::endl;
		}

		PlayerState* HandleInput(Player& player) override;

		void Update(Player& /*player*/, float /*deltaTime*/) override
		{
		}
	};

	//////////////////////////////////////////////////////////////// FallState = TODO
	class FallState final : public PlayerState
	{
	public:
		void Enter(Player& /*player*/) override
		{
			std::cout << "Entered FallState" << std::endl;
		}

		PlayerState* HandleInput(Player& player) override;

		void Update(Player& /*player*/, float /*deltaTime*/) override
		{
		}

	};

	//////////////////////////////////////////////////////////////// ShootState = TODO
	class ShootState final : public PlayerState
	{
	public:
		explicit ShootState(bool wasMoving = false)
			: m_WasMoving(wasMoving) {}

		void Enter(Player& /*player*/) override
		{
			m_Timer = SHOOT_DURATION;
			// TODO: spawn projectile here
			std::cout << "Entered ShootState" << std::endl;
		}

		PlayerState* HandleInput(Player& /*player*/) override
		{
			return nullptr;
		}

		void Update(Player& player, float deltaTime) override
		{
			m_Timer -= deltaTime;
			if (m_Timer <= 0.f)
			{
				if (m_WasMoving && player.GetMoveDirX() != 0.f)
					player.ChangeState(new MoveState{});
				else
					player.ChangeState(new IdleState{});
			}
		}

	private:
		float m_Timer{ 0.f };
		bool  m_WasMoving{ false };
		static constexpr float SHOOT_DURATION = 0.3f;
	};

	//////////////////////////////////////////////////////////////// TakeDmgState = USED
	class TakeDmgState final : public PlayerState
	{
	public:
		void Enter(Player& player) override
		{
			float dir = 1.f;
			player.SetVelocity(dir * KNOCKBACK_VX, 0.f);
			std::cout << "Entered TakeDmgState" << std::endl;
		}

		PlayerState* HandleInput(Player& /*player*/) override
		{
			return nullptr;
		}

		void Update(Player& player, float deltaTime) override
		{
			GameObject* go = player.GetGameObject();
			const auto& POS = go->GetWorldPosition();

			float newX = POS.x + player.GetVelocityX() * deltaTime;
			go->SetLocalPosition(newX, POS.y);

			m_Timer -= deltaTime;
			if (m_Timer <= 0.f)
				player.ChangeState(new IdleState{});
		}

	private:
		float m_Timer{ 0.f };
		const float KNOCKBACK_VX = -150.f;
	};

	//////////////////////////////////////////////////////////////// HandleInput()

	inline PlayerState* IdleState::HandleInput(Player& player)
	{
		if (player.WantsShoot()) return new ShootState{ false };
		if (player.WantsJump())  return new JumpState{};
		if (player.GetMoveDirX() != 0.f) return new MoveState{};
		return nullptr;
	}

	inline PlayerState* MoveState::HandleInput(Player& player)
	{
		if (player.WantsShoot()) return new ShootState{ true };
		if (player.WantsJump())  return new JumpState{};
		if (player.GetMoveDirX() == 0.f) return new IdleState{};
		return nullptr;
	}

	inline PlayerState* JumpState::HandleInput(Player& player)
	{
		if (player.WantsShoot())         return new ShootState{ false };
		if (player.GetVelocityY() >= 0.f) return new FallState{};
		return nullptr;
	}

	inline PlayerState* FallState::HandleInput(Player& player)
	{
		if (player.IsGrounded()) return new IdleState{};
		return nullptr;
	}

}