#ifndef LOGGINGSOUNDSYSTEM_H
#define LOGGINGSOUNDSYSTEM_H

#include <memory>
#include <iostream>

#include "ISoundSystem.h"

namespace dae
{
	class LoggingSoundSystem final : public ISoundSystem
	{
	public:
		explicit LoggingSoundSystem(std::unique_ptr<ISoundSystem> wrapped)
			: m_pWrapped(std::move(wrapped))
		{
		}

		sound_id AddSound(const std::string& filePath) override
		{
			const sound_id id = m_pWrapped->AddSound(filePath);
			return id;
		}

		void Play(sound_id id, float volume) override
		{
			m_pWrapped->Play(id, volume);
		}

	private:
		std::unique_ptr<ISoundSystem> m_pWrapped;
	};
}
#endif