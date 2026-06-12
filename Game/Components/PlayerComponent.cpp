#include "Components/PlayerComponent.h"
#include "States/PlayerState.h"
#include "PlayerStates.h"
#include "GameObject.h"
#include "Components/HealthComponent.h"
#include "Components/PhysicsComponent.h"
#include "TextureComponent.h"
#include <cassert>
#include <cmath>

namespace dae
{
	PlayerComponent::PlayerComponent(GameObject* owner, float movementSpeed)
		: Component(owner), m_MoveSpeed{movementSpeed}
	{
		m_pPhysics = owner->GetComponent<PhysicsComponent>();

		m_pCurrentState = new IdleState{};
		m_pCurrentState->Enter(*this);
		InitObserver();
	}

	PlayerComponent::~PlayerComponent()
	{
		if (m_pCurrentState)
		{
			m_pCurrentState->Exit(*this);
			delete m_pCurrentState;
			m_pCurrentState = nullptr;
		}
	}

	void PlayerComponent::Update(float deltaTime)
	{
		if (!m_pCurrentState) return;

		if (PlayerState* next = m_pCurrentState->HandleInput(*this))
		{
			ChangeState(next);
		}

		m_pCurrentState->Update(*this, deltaTime);

		m_WantsJump = false;
		m_WantsShoot = false;
	}

	void PlayerComponent::Notify(const Event& event, GameObject* /*actor*/)
	{
		if (event.id == EVENT_PLAYER_HIT || event.id == EVENT_PLAYER_DIED)
		{
			if (dynamic_cast<TakeDmgState*>(m_pCurrentState) == nullptr)
			{
				ChangeState(new TakeDmgState{});
			}
		}
	}

	// ── Input ────────────────────────────────────────────────────────────────
	void PlayerComponent::SetMoveInput(float dirX)
	{
		m_MoveDirX = dirX;
		if (dirX != 0.f)
			if (auto* tex = GetOwner()->GetComponent<TextureComponent>())
				tex->FlipX = dirX > 0.f;
	}
	void PlayerComponent::SetJumpInput(bool wants) { if (wants) m_WantsJump = true; }
	void PlayerComponent::SetShootInput(bool wants) { if (wants) m_WantsShoot = true; }

	// ── Physics queries ──────────────────────────────────────────────────────
	bool  PlayerComponent::IsGrounded()   const { return m_pPhysics ? m_pPhysics->IsGrounded() : false; }
	float PlayerComponent::GetVelocityY() const { return m_pPhysics ? m_pPhysics->GetVelocity().y : 0.f; }

	// ── Physics actions ──────────────────────────────────────────────────────
	void PlayerComponent::RequestMove(float dirX)
	{
		if (m_pPhysics) m_pPhysics->RequestMoveX(dirX, m_MoveSpeed);
	}

	void PlayerComponent::StopMove()
	{
		if (m_pPhysics) m_pPhysics->ClearMoveX();
	}

	void PlayerComponent::Jump(float speed)
	{
		if (m_pPhysics) m_pPhysics->SetVelocityY(-speed);
	}

	void PlayerComponent::ApplyKnockback(float vx)
	{
		if (m_pPhysics) m_pPhysics->SetVelocityX(vx);
	}

	void PlayerComponent::StopHorizontal()
	{
		if (m_pPhysics) m_pPhysics->SetVelocityX(0.f);
	}

	void PlayerComponent::DecayHorizontalVelocity(float deltaTime, float drag)
	{
		if (!m_pPhysics) return;
		const float vx = m_pPhysics->GetVelocity().x;
		if (std::abs(vx) > 1.f)
			m_pPhysics->SetVelocityX(vx * std::pow(drag, deltaTime));
		else
			m_pPhysics->SetVelocityX(0.f);
	}

	// ── State machine ────────────────────────────────────────────────────────
	void PlayerComponent::ChangeState(PlayerState* newState)
	{
		assert(newState != nullptr);
		if (m_pCurrentState)
		{
			m_pCurrentState->Exit(*this);
			delete m_pCurrentState;
		}
		m_pCurrentState = newState;
		m_pCurrentState->Enter(*this);
	}

	const char* PlayerComponent::GetCurrentStateName() const
	{
		if (!m_pCurrentState) return "None";
		if (dynamic_cast<IdleState*>    (m_pCurrentState)) return "Idle";
		if (dynamic_cast<MoveState*>    (m_pCurrentState)) return "Move";
		if (dynamic_cast<JumpState*>    (m_pCurrentState)) return "Jump";
		if (dynamic_cast<FallState*>    (m_pCurrentState)) return "Fall";
		if (dynamic_cast<ShootState*>   (m_pCurrentState)) return "Shoot";
		if (dynamic_cast<TakeDmgState*> (m_pCurrentState)) return "TakeDmg";
		return "Unknown";
	}

	void PlayerComponent::InitObserver()
	{
		if (auto* health = GetOwner()->GetComponent<HealthComponent>())
			health->AddObserver(this);
	}
}