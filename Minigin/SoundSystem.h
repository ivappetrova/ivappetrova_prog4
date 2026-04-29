#pragma once
#include "ISoundSystem.h"
#include <memory>

namespace dae
{
	class SoundSystem final : public ISoundSystem
	{
	public:
		SoundSystem();
		~SoundSystem() override;

		sound_id AddSound(const std::string& filePath) override;
		void Play(sound_id id, float volume) override;

		SoundSystem(const SoundSystem&) = delete;
		SoundSystem(SoundSystem&&) = delete;
		SoundSystem& operator=(const SoundSystem&) = delete;
		SoundSystem& operator=(SoundSystem&&) = delete;

	private:
		struct Impl;
		std::unique_ptr<Impl> m_pImpl;
	};
}