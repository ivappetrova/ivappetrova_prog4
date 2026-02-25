#pragma once
#include "Component.h"

namespace dae
{
	class RotatorComponent final : public Component
	{
	public:
		RotatorComponent(GameObject* owner, float radius, float speed, float centerX = 0.f, float centerY = 0.f);
		void Update(float deltaTime) override;

	private:
		float m_Radius{};
		float m_Speed{};
		float m_Angle{};
		float m_CenterX{};
		float m_CenterY{};
	};
}