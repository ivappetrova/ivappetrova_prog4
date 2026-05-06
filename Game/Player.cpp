#include "Player.h"
#include "PlayerState.h"
#include "PlayerStates.h"
#include "GameObject.h"
#include "HealthComponent.h"
#include <cassert>

namespace dae
{

	Player::Player(GameObject* owner)
		: Component(owner)
	{
		m_pCurrentState = new IdleState{};
		m_pCurrentState->Enter(*this);
		InitObserver();
	}

	Player::~Player()
	{
		if (m_pCurrentState)
		{
			m_pCurrentState->Exit(*this);
			delete m_pCurrentState;
			m_pCurrentState = nullptr;
		}
	}

	void Player::Update(float deltaTime)
	{
		if (!m_pCurrentState) return;

		if (PlayerState* next = m_pCurrentState->HandleInput(*this))
			ChangeState(next);

		m_pCurrentState->Update(*this, deltaTime);

		ClearFrameInput();
	}

	void Player::Notify(const Event& event, GameObject* /*actor*/)
	{
		if (event.id == EVENT_PLAYER_HIT || event.id == EVENT_PLAYER_DIED)
		{
			if (dynamic_cast<TakeDmgState*>(m_pCurrentState) == nullptr)
				ChangeState(new TakeDmgState{});
		}
	}

	void Player::SetMoveInput(float dirX)
	{
		m_MoveDirX = dirX;
	}

	void Player::SetJumpInput(bool wants)
	{
		if (wants) m_WantsJump = true;
	}

	void Player::SetShootInput(bool wants)
	{
		if (wants) m_WantsShoot = true;
	}

	void Player::SetVelocity(float vx, float vy)
	{
		m_VelocityX = vx;
		m_VelocityY = vy;
	}

	void Player::AddVelocity(float ax, float ay)
	{
		m_VelocityX += ax;
		m_VelocityY += ay;
	}

	void Player::ChangeState(PlayerState* newState)
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

	const char* Player::GetCurrentStateName() const
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

	void Player::InitObserver()
	{
		if (auto* health = GetOwner()->GetComponent<HealthComponent>())
			health->AddObserver(this);
	}

	void Player::ClearFrameInput()
	{
		m_MoveDirX = 0.f;   // reset every frame – MoveCommand re-sets it if key is still held
		m_WantsJump = false;
		m_WantsShoot = false;
	}

} 