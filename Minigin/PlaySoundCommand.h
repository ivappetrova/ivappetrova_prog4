#pragma once
#include "Command.h"
#include "ServiceLocator.h"

namespace dae
{
	class PlaySoundCommand final : public Command
	{
	public:
		PlaySoundCommand(sound_id id, float volume)
			: m_Id(id), m_Volume(volume)
		{
		}

		void Execute() override
		{
			ServiceLocator::GetSoundSystem().Play(m_Id, m_Volume);
		}

	private:
		sound_id m_Id;
		float    m_Volume;
	};
}