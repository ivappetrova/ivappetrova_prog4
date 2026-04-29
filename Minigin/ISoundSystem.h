#pragma once
#include <string>

namespace dae
{
	using sound_id = unsigned short;

	class ISoundSystem
	{
	public:
		virtual ~ISoundSystem() = default;

		virtual sound_id AddSound(const std::string& filePath) = 0;
		virtual void Play(sound_id id, float volume) = 0;

		// Rule of 5
		ISoundSystem(const ISoundSystem&) = delete;
		ISoundSystem(ISoundSystem&&) = delete;
		ISoundSystem& operator=(const ISoundSystem&) = delete;
		ISoundSystem& operator=(ISoundSystem&&) = delete;

	protected:
		ISoundSystem() = default;
	};
}