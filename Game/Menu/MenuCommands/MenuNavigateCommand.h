#ifndef MENU_NAVIGATE_COMMAND_H
#define MENU_NAVIGATE_COMMAND_H

#include "Command.h"
#include "Menu/MenuState.h"

namespace dae
{
	class MenuState;

	class MenuNavigateCommand final : public Command
	{
	public:
		MenuNavigateCommand(MenuState& menu, int delta)
			: m_Menu(menu), m_Delta(delta) {}

		void Execute() override
		{
			m_Menu.Navigate(m_Delta);
		}

	private:
		MenuState& m_Menu;
		int m_Delta;
	};
}
#endif