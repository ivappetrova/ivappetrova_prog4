#ifndef SOUND_OBSERVER_H
#define SOUND_OBSERVER_H

#include "IObserver.h"
#include "Event.h"
#include "ServiceLocator.h"
#include "ISoundSystem.h"

namespace dae
{
	class SoundObserver final : public IObserver
	{
	public:
		SoundObserver(sound_id id, EventId listenFor, float volume = 0.6f)
			: m_SoundId(id), m_ListenFor(listenFor), m_Volume(volume) 
		{
		}

		void Notify(const Event& event, GameObject*) override
		{
			if (event.id == m_ListenFor)
			{ 
				ServiceLocator::GetSoundSystem().Play(m_SoundId, m_Volume);
			}
		}
	private:
		sound_id m_SoundId{};
		EventId m_ListenFor{};
		float m_Volume{};
	};
}
#endif