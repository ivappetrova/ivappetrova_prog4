#pragma once
#include "Component.h"
#include "IObserver.h"
#include "TextComponent.h"
#include "Event.h"

namespace dae
{
	class PlayerDiedDisplayComponent final : public Component, public IObserver
	{
	public:
		PlayerDiedDisplayComponent(GameObject* owner)
			: Component(owner) {
		}

		void Notify(const Event& event, GameObject* /*actor*/) override
		{
			if (event.id == EVENT_PLAYER_DIED)
			{
				if (auto* text = GetOwner()->GetComponent<TextComponent>())
					text->SetText("Player Died!");
			}
		}
	};
}