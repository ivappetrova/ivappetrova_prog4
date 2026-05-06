#include "Transform.h"

void dae::Transform::SetLocalPosition(const glm::vec3& position)
{
	m_LocalPosition = position;
	m_IsDirty = true;
}

void dae::Transform::SetWorldPosition(const glm::vec3& pos) const
{
	m_WorldPosition = pos;
	m_IsDirty = false;
}