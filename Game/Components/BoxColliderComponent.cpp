#include "Components/BoxColliderComponent.h"
#include "GameObject.h"

namespace dae
{
	BoxColliderComponent::BoxColliderComponent(GameObject* owner, float width, float height)
		: Component(owner), m_Width(width), m_Height(height)
	{
	}

	glm::vec2 BoxColliderComponent::WorldOrigin() const
	{
		const glm::vec2& WORLD_ORIGIN { GetOwner()->GetWorldPosition() };
		return { WORLD_ORIGIN.x, WORLD_ORIGIN.y - m_Height };
	}

	glm::vec2 BoxColliderComponent::GetTopLeft() const 
	{
		return WorldOrigin(); 
	}

	glm::vec2 BoxColliderComponent::GetTopRight() const 
	{ 
		const glm::vec2 WORLD_ORIGIN{ WorldOrigin() };
		return { WORLD_ORIGIN.x + m_Width, WORLD_ORIGIN.y };
	}

	glm::vec2 BoxColliderComponent::GetBottomLeft() const 
	{ 
		const glm::vec2 WORLD_ORIGIN{ WorldOrigin() };
		return { WORLD_ORIGIN.x, WORLD_ORIGIN.y + m_Height };
	}

	glm::vec2 BoxColliderComponent::GetBottomRight() const 
	{ 
		const glm::vec2 WORLD_ORIGIN{ WorldOrigin() };
		return { WORLD_ORIGIN.x + m_Width, WORLD_ORIGIN.y + m_Height };
	}

	glm::vec2 BoxColliderComponent::GetCenter() const 
	{ 
		const glm::vec2 WORLD_ORIGIN{ WorldOrigin() };
		return { WORLD_ORIGIN.x + m_Width * 0.5f, WORLD_ORIGIN.y + m_Height * 0.5f };
	}

	bool BoxColliderComponent::Overlaps(const BoxColliderComponent& other) const
	{
		const glm::vec2 TOP_LEFT{ GetTopLeft() };
		const glm::vec2 OTHER_TOP_LEFT{ other.GetTopLeft() };

		return TOP_LEFT.x < OTHER_TOP_LEFT.x + other.m_Width &&
			   TOP_LEFT.x + m_Width  > OTHER_TOP_LEFT.x &&
			   TOP_LEFT.y < OTHER_TOP_LEFT.y + other.m_Height &&
			   TOP_LEFT.y + m_Height > OTHER_TOP_LEFT.y;
	}
}