#include "RotatorComponent.h"
#include "GameObject.h"
#include <numbers>


dae::RotatorComponent::RotatorComponent(GameObject* owner, float radius, float speed)
	: Component(owner), m_Radius(radius), m_Speed(speed), m_Angle(0.f)
{
}

void dae::RotatorComponent::Update(float deltaTime)
{
	m_Angle += m_Speed * deltaTime;

	// clamp angle 
	m_Angle = std::fmod(m_Angle, 2.f * std::numbers::pi_v<float>);

	GetOwner()->SetLocalPosition(std::cos(m_Angle) * m_Radius, std::sin(m_Angle) * m_Radius);
}