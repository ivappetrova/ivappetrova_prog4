#ifndef LIVESDISPLAYCOMPONENT_H
#define LIVESDISPLAYCOMPONENT_H

#include <string>

#include "Component.h"
#include "IObserver.h"
#include "TextComponent.h"
#include "Event.h"
#include "GameObject.h"

namespace dae
{
	class HealthDisplayComponent final : public Component, public IObserver
	{
	public:
		HealthDisplayComponent(GameObject* owner, HealthComponent* pHealth)
			: Component(owner), m_Lives(pHealth ? pHealth->GetHP() : 0)
		{
		}

		void Notify(const Event& event, GameObject* actor) override
		{
			if ((event.id == EVENT_PLAYER_HIT || event.id == EVENT_PLAYER_DIED) && actor)
			{
				if (auto* pHealth = actor->GetComponent<HealthComponent>())
				{
					m_Lives = pHealth->GetHP();
				}
				UpdateText();
			}
		}

		int GetLives() const { return m_Lives; }

	private:
		int m_Lives;

		void UpdateText()
		{
			if (auto* text = GetOwner()->GetComponent<TextComponent>())
			{
				text->SetText("Lives: " + std::to_string(m_Lives));
			}
		}
	};
}
#endif