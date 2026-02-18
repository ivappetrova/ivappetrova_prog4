#include "FPSComponent.h"
#include "GameObject.h"
#include "TextComponent.h"

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
		const int FPS = m_FrameCount;
		m_pTextComponent->SetText("FPS: " + std::to_string(FPS));

		m_ElapsedTime = 0.f;
		m_FrameCount = 0;
	}
}
