#include "RotatorComponent.h"
#include "GameObject.h"
#include <cmath>


dae::RotatorComponent::RotatorComponent(GameObject* owner, float radius, float speed)
	: Component(owner), m_Radius(radius), m_Speed(speed), m_Angle(0.f)
{
}

void dae::RotatorComponent::Update(float deltaTime)
{


	m_Angle += m_Speed * deltaTime;

	// clamp angle 
	if (m_Angle >= 2.f * m_PI)
	{
		m_Angle -= 2.f * m_PI;
	}

	GetOwner()->SetLocalPosition(std::cos(m_Angle) * m_Radius, std::sin(m_Angle) * m_Radius);
}