#ifndef SCORE_COMPONENT
#define SCORE_COMPONENT

#include "Component.h"
#include "Subject.h"
#include "Event.h"

namespace dae
{
	class ScoreComponent final : public Component, public Subject
	{
	public:
		ScoreComponent(GameObject* owner)
			: Component(owner), m_Score{}
		{
		}

		void AddPoints(int points)
		{
			m_Score += points;
			NotifyObservers(Event{ EVENT_PLAYER_GET_POINTS }, GetOwner());
		}

		int GetScore() const 
		{
			return m_Score; 
		}

	private:
		int m_Score;
	};
}
#endif