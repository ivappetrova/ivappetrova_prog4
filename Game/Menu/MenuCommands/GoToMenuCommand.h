#ifndef GO_TO_MENU_COMMAND_H
#define GO_TO_MENU_COMMAND_H

#include "Command.h"
#include "Menu/MenuState.h"
#include "GameStateManager.h"
#include <memory>

namespace dae
{
	class GameStateManager;

	// Switches back to the main menu. Bind to ESCAPE in every game state.
	class GoToMenuCommand final : public Command
	{
	public:
		GoToMenuCommand(GameStateManager& gsm, float windowWidth, float windowHeight)
			: m_GSM(gsm), m_WindowWidth(windowWidth), m_WindowHeight(windowHeight) 
		{
		}

		void Execute() override
		{
			m_GSM.SwitchTo(std::make_unique<MenuState>(m_GSM, m_WindowWidth, m_WindowHeight));
		}

	private:
		GameStateManager& m_GSM;
		float m_WindowWidth;
		float m_WindowHeight;
	};
}
#endif