#ifndef SERVICELOCATOR_H
#define SERVICELOCATOR_H

#include <memory>

#include "ISoundSystem.h"
#include "NullSoundSystem.h"

namespace dae
{
	// Gives a global point of access to the active sound system
	class ServiceLocator final
	{
	public:
		static void RegisterSoundSystem(std::unique_ptr<ISoundSystem> soundSystem)
		{
			m_pSoundSystem = std::move(soundSystem);
		}

		[[nodiscard]] static ISoundSystem& GetSoundSystem()
		{
			if (!m_pSoundSystem)
			{
				m_pSoundSystem = std::make_unique<NullSoundSystem>();
			}
			return *m_pSoundSystem;
		}

		ServiceLocator() = delete;

	private:
		// Starts as a NullSoundSystem -> always in a valid state
		inline static std::unique_ptr<ISoundSystem> m_pSoundSystem
		{ 
			std::make_unique<NullSoundSystem>()
		};
	};
}
#endif