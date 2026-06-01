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
			const sound_id ID = m_pWrapped->AddSound(filePath);
			std::cout << "[LoggingSoundSystem] AddSound: " << filePath << " -> id: " << ID << std::endl;
			return ID;
		}

		void Play(sound_id id, float volume) override
		{
			m_pWrapped->Play(id, volume);
			std::cout << "[LoggingSoundSystem] Play: id=" << id << " volume=" << volume << std::endl;
		}

	private:
		std::unique_ptr<ISoundSystem> m_pWrapped;
	};
}
#endif