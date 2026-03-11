#include <SDL3/SDL.h>
#include "InputManager.h"
#include "Command.h"

namespace dae
{
	bool InputManager::ProcessInput()
	{
		// Clear per-frame keyboard tracking
		m_KeysDownThisFrame.clear();
		m_KeysUpThisFrame.clear();

		// Poll SDL events (keyboard / mouse / window)
		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			if (e.type == SDL_EVENT_QUIT)
				return false;

			if (e.type == SDL_EVENT_KEY_DOWN && !e.key.repeat)
				m_KeysDownThisFrame.push_back(e.key.scancode);

			if (e.type == SDL_EVENT_KEY_UP)
				m_KeysUpThisFrame.push_back(e.key.scancode);
		}

		// Update all active controllers 
		for (size_t index{}; index < MAX_CONTROLLERS; ++index)
		{
			if (m_pControllers[index])
			{
				m_pControllers[index]->Update();
			}
		}

		// Execute controller commands
		for (auto& binding : m_ControllerBindings)
		{
			if (!m_pControllers[binding.controllerIndex]) continue;
			const auto& ctrl = *m_pControllers[binding.controllerIndex];

			bool triggered = false;
			switch (binding.keyState)
			{
			case Controller::KeyState::Down:
				triggered = ctrl.IsDown(binding.button);
				break;
			case Controller::KeyState::Up:
				triggered = ctrl.IsUp(binding.button);
				break;
			case Controller::KeyState::Pressed:
				triggered = ctrl.IsPressed(binding.button);
				break;
			}

			if (triggered)
			{
				binding.command->Execute();
			}
		}

		// Execute keyboard commands
		const bool* keyboardState = SDL_GetKeyboardState(nullptr);

		for (auto& binding : m_KeyboardBindings)
		{
			bool triggered = false;
			switch (binding.keyState)
			{
			case KeyState::Down:
			{
				for (SDL_Scancode sc : m_KeysDownThisFrame)
					if (sc == binding.key) { triggered = true; break; }
				break;
			}
			case KeyState::Up:
			{
				for (SDL_Scancode sc : m_KeysUpThisFrame)
					if (sc == binding.key) { triggered = true; break; }
				break;
			}
			case KeyState::Pressed:
				triggered = keyboardState[binding.key];
				break;
			}

			if (triggered)
				binding.command->Execute();
		}

		return true;
	}

	Controller& InputManager::GetOrCreateController(unsigned int index)
	{
		if (!m_pControllers[index])
		{
			m_pControllers[index] = std::make_unique<Controller>(index);
		}
		return *m_pControllers[index];
	}

	void InputManager::BindControllerCommand(unsigned int controllerIndex, Controller::Button button, 
											 Controller::KeyState keyState, std::unique_ptr<Command> command)
	{
		GetOrCreateController(controllerIndex);

		m_ControllerBindings.push_back({controllerIndex, button, keyState, std::move(command) });
	}

	void InputManager::UnbindControllerCommand(unsigned int controllerIndex, Controller::Button button, 
												Controller::KeyState keyState)
	{
		m_ControllerBindings.erase(
			std::remove_if(m_ControllerBindings.begin(), m_ControllerBindings.end(),
				[&](const ControllerBinding& b)
				{
					return b.controllerIndex == controllerIndex
						&& b.button == button
						&& b.keyState == keyState;
				}),
			m_ControllerBindings.end());
	}

	void InputManager::BindKeyboardCommand(SDL_Scancode key, KeyState keyState, std::unique_ptr<Command> command)
	{
		m_KeyboardBindings.push_back({ key, keyState, std::move(command) });
	}

	void InputManager::UnbindKeyboardCommand(SDL_Scancode key, KeyState keyState)
	{
		m_KeyboardBindings.erase(
			std::remove_if(m_KeyboardBindings.begin(), m_KeyboardBindings.end(),
				[&](const KeyboardBinding& b)
				{
					return b.key == key && b.keyState == keyState;
				}),
			m_KeyboardBindings.end());
	}

	void InputManager::UnbindAll()
	{
		m_ControllerBindings.clear();
		m_KeyboardBindings.clear();
	}
}