#pragma once
#include <memory>
#include <vector>
#include <map>
#include <SDL3/SDL.h>
#include "Singleton.h"
#include "Controller.h"
#include "Command.h"

namespace dae
{

	class InputManager final : public Singleton<InputManager>
	{
	public:
		// Keyboard key states
		enum class KeyState
		{
			Down,   
			Up,     
			Pressed,
		};

		bool ProcessInput();

		// Controller bindings
		void BindControllerCommand(unsigned int controllerIndex, Controller::Button button, Controller::KeyState keyState, 
								   std::unique_ptr<Command> command);
		void UnbindControllerCommand(unsigned int controllerIndex, Controller::Button button, Controller::KeyState keyState);

		// Keyboard bindings - SDL
		void BindKeyboardCommand(SDL_Scancode key, KeyState keyState, std::unique_ptr<Command> command);
		void UnbindKeyboardCommand(SDL_Scancode key, KeyState keyState);

		void UnbindAll();

	private:
		friend class Singleton<InputManager>;
		InputManager() = default;

		//////////////////////////////////////////////////////////////////// Controller state
		struct ControllerBinding
		{
			unsigned int          controllerIndex;
			Controller::Button    button;
			Controller::KeyState  keyState;
			std::unique_ptr<Command> command;
		};

		static constexpr unsigned int MAX_CONTROLLERS = 4;
		std::unique_ptr<Controller> m_pControllers[MAX_CONTROLLERS];

		Controller& GetOrCreateController(unsigned int index);

		std::vector<ControllerBinding> m_ControllerBindings;

		//////////////////////////////////////////////////////////////////// Keyboard state
		struct KeyboardBinding
		{
			SDL_Scancode              key;
			KeyState                  keyState;
			std::unique_ptr<Command>  command;
		};

		std::vector<KeyboardBinding> m_KeyboardBindings;
		std::vector<SDL_Scancode> m_KeysDownThisFrame;
		std::vector<SDL_Scancode> m_KeysUpThisFrame;
	};
}