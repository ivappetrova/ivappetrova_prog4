#include "FPSComponent.h"
#include "GameObject.h"
#include "TextComponent.h"
#include <sstream>
#include <iomanip>

dae::FPSComponent::FPSComponent(GameObject* owner)
	: Component(owner)
{
	m_pTextComponent = owner->GetComponent<TextComponent>();
}

void dae::FPSComponent::Update(float deltaTime)
{
	if (!m_pTextComponent) return;

	m_ElapsedTime += deltaTime;
	++m_FrameCount;

	if (m_ElapsedTime >= 1.0f)
	{
		const float FPS = m_FrameCount;
		std::ostringstream stream;
		stream << std::fixed << std::setprecision(2) << FPS;
		std::string fpsStr = stream.str();

		m_pTextComponent->SetText("FPS: " + fpsStr);

		m_ElapsedTime = 0.f;
		m_FrameCount = 0;
	}
}
