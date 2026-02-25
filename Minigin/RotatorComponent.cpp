#include "RotatorComponent.h"
#include "GameObject.h"
#include <cmath>

dae::RotatorComponent::RotatorComponent(GameObject* owner, float radius, float speed, float centerX, float centerY)
	: Component(owner), m_Radius(radius), m_Speed(speed), m_Angle(0.f), m_CenterX(centerX), m_CenterY(centerY)
{
}

void dae::RotatorComponent::Update(float deltaTime)
{
	m_Angle += m_Speed * deltaTime;
	GetOwner()->SetLocalPosition(
		m_CenterX + std::cos(m_Angle) * m_Radius,
		m_CenterY + std::sin(m_Angle) * m_Radius
	);
}