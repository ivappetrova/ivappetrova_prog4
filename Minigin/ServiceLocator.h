#pragma once
#include "ISoundSystem.h"
#include "NullSoundSystem.h"
#include <memory>

namespace dae
{
	// Gives a global point of access to the active sound system
	class ServiceLocator final
	{
	public:
		[[nodiscard]] static ISoundSystem& GetSoundSystem()
		{
			return *m_pSoundSystem;
		}

		static void RegisterSoundSystem(std::unique_ptr<ISoundSystem> soundSystem)
		{
			m_pSoundSystem = soundSystem
				? std::move(soundSystem)
				: std::make_unique<NullSoundSystem>();
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