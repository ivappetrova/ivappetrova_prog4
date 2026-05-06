#pragma once
#include "Component.h"
#include "Subject.h"
#include "Event.h"

namespace dae
{
	class PlayerPointsComponent final : public Component, public Subject
	{
	public:
		PlayerPointsComponent(GameObject* owner)
			: Component(owner), m_Points{} 
		{
		}

		void AddPoints(int points)
		{
			
			m_Points += points;
			NotifyObservers(Event{ EVENT_PLAYER_GET_POINTS }, GetOwner());
		}

		int GetPoints() const { return m_Points; }

	private:
		int m_Points;
	};
}