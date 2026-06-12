#ifndef BOXCOLLIDER_H
#define BOXCOLLIDER_H

#include "Component.h"
#include <glm/vec2.hpp>

namespace dae
{
	class BoxColliderComponent final : public Component
	{
	public:
		BoxColliderComponent(GameObject* owner, float width, float height);

		glm::vec2 GetTopLeft() const;
		glm::vec2 GetTopRight() const;
		glm::vec2 GetBottomLeft() const;
		glm::vec2 GetBottomRight() const;
		glm::vec2 GetCenter() const;

		float GetWidth()  const { return m_Width; }
		float GetHeight() const { return m_Height; }

		// Collision check
		bool Overlaps(const BoxColliderComponent& other) const;

	private:
		glm::vec2 WorldOrigin() const;

		float m_Width;
		float m_Height;
	};
}
#endif