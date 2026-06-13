#ifndef SUBJECT_H
#define SUBJECT_H

#include <vector>

#include "IObserver.h"
#include "Event.h"

namespace dae
{
	class Subject
	{
	public:
		void AddObserver(IObserver* observer)
		{
			m_pObservers.push_back(observer);
		}

		void RemoveObserver(IObserver* observer)
		{
			m_pObservers.erase( std::remove(m_pObservers.begin(), m_pObservers.end(), observer), m_pObservers.end());
		}

	protected:
		void NotifyObservers(const Event& event, GameObject* actor)
		{
			for (auto* pObserver : m_pObservers)
			{
				pObserver->Notify(event, actor);
			}
		}

	private:
		std::vector<IObserver*> m_pObservers;
	};
}
#endif