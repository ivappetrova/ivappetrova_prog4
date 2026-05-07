#ifndef ISOUNDSYSTEM_H
#define ISOUNDSYSTEM_H

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

	};
}
#endif