#pragma once
#include "Component.h"

namespace dae
{
	class TextComponent;

	class FPSComponent final : public Component
	{
	public:
		explicit FPSComponent(GameObject* owner);

		void Update(float deltaTime) override;

	private:
		TextComponent* m_pTextComponent{ nullptr };

		float m_ElapsedTime{};
		float m_FrameCount{};
	};
}
