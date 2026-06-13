#ifndef MUTE_COMMAND_H
#define MUTE_COMMAND_H

#include "Command.h"
#include "ServiceLocator.h"

namespace dae
{
	class MuteCommand final : public Command
	{
	public:
		void Execute() override
		{
			m_IsMuted = !m_IsMuted;
			ServiceLocator::GetSoundSystem().SetMuted(m_IsMuted);
		}

	private:
		bool m_IsMuted{ false };
	};
}
#endif