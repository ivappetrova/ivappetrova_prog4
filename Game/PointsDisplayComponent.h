#pragma once
#include "Component.h"
#include "IObserver.h"
#include "TextComponent.h"
#include "Event.h"
#include <string>

namespace dae
{
	class PointsDisplayComponent final : public Component, public IObserver
	{
	public:
		PointsDisplayComponent(GameObject* owner)
			: Component(owner), m_Points{}
		{
		}

		void Notify(const Event& event, GameObject* actor) override
		{
			if (event.id == EVENT_PLAYER_GET_POINTS)
			{
				if (auto* pts = actor->GetComponent<PlayerPointsComponent>())
					m_Points = pts->GetPoints();
				UpdateText();
			}
		}

		int GetPoints() const { return m_Points; }

	private:
		int m_Points;

		void UpdateText()
		{
			if (auto* text = GetOwner()->GetComponent<TextComponent>())
				text->SetText("Points: " + std::to_string(m_Points));
		}
	};
}