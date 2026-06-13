#include "PhysicsComponent.h"
#include "LevelCollisionComponent.h"
#include "BoxColliderComponent.h"
#include "GameObject.h"
#include <algorithm>
#include <cfloat>

namespace dae
{
	PhysicsComponent::PhysicsComponent(GameObject* owner, float screenHeight)
		: Component(owner), m_WindowHeight{ screenHeight }
	{
	}

	void PhysicsComponent::Update(float deltaTime)
	{
		// Apply gravity
		if (!m_IsGrounded)
		{
			m_Velocity.y += m_GRAVITY * deltaTime;
			if (m_Velocity.y > m_MAX_FALL_SPEED)
			{
				m_Velocity.y = m_MAX_FALL_SPEED;
			}
		}

		ResolveCollisions(deltaTime);
	}

	void PhysicsComponent::RequestMoveX(float direction, float speed)
	{
		m_MoveRequest = direction;  // -1, 0, or +1
		m_MoveSpeed = speed;
	}

	void PhysicsComponent::ResolveCollisions(float deltaTime)
	{
		auto* pOwner{ GetOwner() };
		const glm::vec3 POS{ pOwner->GetWorldPosition() };

		float newX{ POS.x + m_Velocity.x * deltaTime };
		float newY{ POS.y + m_Velocity.y * deltaTime };

		m_IsGrounded = false;

		if (!m_pLevelCollision)
		{
			pOwner->SetLocalPosition(newX, newY);
			return;
		}

		BoxColliderComponent* pBoxCollider = pOwner->GetComponent<BoxColliderComponent>();
		if (!pBoxCollider)
		{
			pOwner->SetLocalPosition(newX, newY);
			return;
		}

		const float BOX_COLLIDER_WIDTH{ pBoxCollider->GetWidth() };
		const float BOX_COLLIDER_HEIGHT{ pBoxCollider->GetHeight() };

		///////////////////////// Vertical Check
		{
			const glm::vec2 BOT_CENTRE{ newX + BOX_COLLIDER_WIDTH * 0.5f, newY };
			constexpr float GROUND_TOLERANCE{ 4.f };

			m_IsGrounded = m_pLevelCollision->IsGrounded(BOT_CENTRE, GROUND_TOLERANCE);

			if (m_Velocity.y > 0.f)
			{
				const float SEARCH_DIST = m_Velocity.y * deltaTime + 2.f;
				const float GROUND_Y = m_pLevelCollision->GroundYBelow(BOT_CENTRE, SEARCH_DIST);
				if (GROUND_Y != FLT_MAX)
				{
					newY = GROUND_Y;
					m_Velocity.y = 0.f;
					m_IsGrounded = true;
				}
			}
			else if (m_Velocity.y < 0.f)
			{
				const glm::vec2 TOP_LEFT{ newX, newY - BOX_COLLIDER_HEIGHT };
				const glm::vec2 TOP_MID{ newX + BOX_COLLIDER_WIDTH * 0.5f, newY - BOX_COLLIDER_HEIGHT };
				const glm::vec2 TOP_RIGHT{ newX + BOX_COLLIDER_WIDTH, newY - BOX_COLLIDER_HEIGHT };

				const float m_CEILING_Y{ 60.f };

				// The player hits his head on the ceiling only if this is the toppest part of the game, otherwise jumps on the platforms
				if ((newY - BOX_COLLIDER_HEIGHT) < m_CEILING_Y &&
					(m_pLevelCollision->Overlaps(TOP_LEFT) ||
						m_pLevelCollision->Overlaps(TOP_MID) ||
						m_pLevelCollision->Overlaps(TOP_RIGHT)))
				{
					newY = POS.y;
					m_Velocity.y = 0.f;
				}
			}
		}

		//////////////////////// Horizontal Check

		// If the move request changed direction or stopped, clear the blocked flag
		if (m_MoveRequest == 0.f || (m_BlockedDirX != 0.f && m_MoveRequest != m_BlockedDirX))
		{
			m_BlockedDirX = 0.f;
		}

		// Suppress movement while still pushing into a blocked wall
		if (m_BlockedDirX != 0.f && m_MoveRequest == m_BlockedDirX)
		{
			m_Velocity.x = 0.f;
		}
		else
		{
			if (m_MoveRequest != 0.f)
			{
				m_Velocity.x = m_MoveRequest * m_MoveSpeed;
			}
			else
			{
				m_Velocity.x = 0.f;
			}
		}

		// Velocity is final=> calculate final newX
		newX = POS.x + m_Velocity.x * deltaTime;

		if (m_Velocity.x != 0.f)
		{
			const bool IS_MOVING_RIGHT{ m_Velocity.x > 0.f };

			// Query three heights along the leading edge (top, mid, bottom — 10 % inset)
			const float LEAD_X{ IS_MOVING_RIGHT ? newX + BOX_COLLIDER_WIDTH : newX };
			const float SEARCH_DIST{ std::abs(m_Velocity.x) * deltaTime + 2.f };

			// Find the nearest wall surface at each height sample
			auto FindWall = [&](float sampleY) -> float
				{
					const glm::vec2 SAMPLE{ LEAD_X, sampleY };
					return m_pLevelCollision->WallXBeside(SAMPLE, SEARCH_DIST, IS_MOVING_RIGHT);
				};

			const float WALL_TOP{ FindWall(newY - BOX_COLLIDER_HEIGHT * 0.9f) };
			const float WALL_MID{ FindWall(newY - BOX_COLLIDER_HEIGHT * 0.5f) };
			const float WALL_BOT{ FindWall(newY - BOX_COLLIDER_HEIGHT * 0.1f) };

			// Pick the most restrictive (closest) wall found
			float wallX{ IS_MOVING_RIGHT ? FLT_MAX : -FLT_MAX };
			if (IS_MOVING_RIGHT)
			{
				wallX = std::min({ WALL_TOP, WALL_MID, WALL_BOT });
			}
			else
			{
				wallX = std::max({ WALL_TOP, WALL_MID, WALL_BOT });
			}

			const bool IS_HITTING_WALL = IS_MOVING_RIGHT ? (wallX != FLT_MAX) : (wallX != -FLT_MAX);
			if (IS_HITTING_WALL)
			{
				// Push flush against the wall and set the dirty flag
				newX = IS_MOVING_RIGHT ? (wallX - BOX_COLLIDER_WIDTH) : wallX;
				m_Velocity.x = 0.f;
				m_BlockedDirX = IS_MOVING_RIGHT ? 1.f : -1.f;
			}
		}

		// Teleport top if the player falls in a gap
		if (newY > m_WindowHeight)
		{
			newY = pBoxCollider->GetHeight();
			m_IsGrounded = false;
		}

		pOwner->SetLocalPosition(newX, newY);
	}
}