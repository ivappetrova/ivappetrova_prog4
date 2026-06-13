#ifndef MENU_CONFIRM_COMMAND_H
#define MENU_CONFIRM_COMMAND_H

#include "Command.h"
#include "Menu/MenuState.h"

namespace dae
{
	class MenuState;

	class MenuConfirmCommand final : public Command
	{
	public:
		explicit MenuConfirmCommand(MenuState& menu) : m_Menu(menu) {}

		void Execute() override
		{
			m_Menu.Confirm();
		}

	private:
		MenuState& m_Menu;
	};
}
#endif