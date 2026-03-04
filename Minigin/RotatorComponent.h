#pragma once
#include "Component.h"

namespace dae
{
	class RotatorComponent final : public Component
	{
	public:
		RotatorComponent(GameObject* owner, float radius, float speed);
		void Update(float deltaTime) override;

	private:
		float m_Radius{};
		float m_Speed{};
		float m_Angle{};

		const float m_PI{ 3.14159265f };
	};
}