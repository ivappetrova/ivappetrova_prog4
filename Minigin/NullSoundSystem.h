#pragma once
#include "ISoundSystem.h"

namespace dae
{
	class NullSoundSystem final : public ISoundSystem
	{
	public:
		sound_id AddSound(const std::string&) override { return 0; }
		void Play(sound_id, float) override {}
	};
}