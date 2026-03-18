#pragma once
#include "Component.h"
#include "IObserver.h"
#include "TextComponent.h"
#include "Event.h"
#include <string>

namespace dae
{
	class LivesDisplayComponent final : public Component, public IObserver
	{
	public:
		LivesDisplayComponent(GameObject* owner, int startingLives = 3)
			: Component(owner), m_Lives(startingLives) {
		}

		void Notify(const Event& event, GameObject* /*actor*/) override
		{
			if (event.id == EVENT_PLAYER_HIT || event.id == EVENT_PLAYER_DIED)
			{
				--m_Lives;
				UpdateText();
			}
		}

		int GetLives() const { return m_Lives; }

	private:
		int m_Lives;

		void UpdateText()
		{
			if (auto* text = GetOwner()->GetComponent<TextComponent>())
				text->SetText("Lives: " + std::to_string(m_Lives));
		}
	};
}