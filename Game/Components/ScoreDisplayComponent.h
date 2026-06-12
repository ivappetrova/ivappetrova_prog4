#ifndef SCORE_DISPLAY_COMPONENT
#define SCORE_DISPLAY_COMPONENT

#include <string>

#include "Component.h"
#include "IObserver.h"
#include "TextComponent.h"
#include "Event.h"
#include "ScoreComponent.h"
#include "GameObject.h"

namespace dae
{
	class ScoreDisplayComponent final : public Component, public IObserver
	{
	public:
		ScoreDisplayComponent(GameObject* owner)
			: Component(owner), m_Score{}
		{
		}

		void Notify(const Event& event, GameObject* actor) override
		{
			if (event.id == EVENT_PLAYER_GET_POINTS)
			{
				if (auto* pScore = actor->GetComponent<ScoreComponent>())
				{
					m_Score = pScore->GetScore();
				}
				UpdateText();
			}
		}

		int GetPoints() const 
		{ 
			return m_Score; 
		}

	private:
		int m_Score;

		void UpdateText()
		{
			if (auto* pText = GetOwner()->GetComponent<TextComponent>())
			{
				pText->SetText("Points: " + std::to_string(m_Score));
			}
		}
	};
}
#endif