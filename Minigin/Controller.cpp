#include "Controller.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <Xinput.h>
#pragma comment(lib, "xinput.lib")

namespace dae
{
	class Controller::ControllerImpl
	{
	public:
		explicit ControllerImpl(unsigned int controllerIndex)
			: m_ControllerIndex(controllerIndex)
		{
			ZeroMemory(&m_PreviousState, sizeof(XINPUT_STATE));
			ZeroMemory(&m_CurrentState, sizeof(XINPUT_STATE));
		}

		void Update()
		{
			CopyMemory(&m_PreviousState, &m_CurrentState, sizeof(XINPUT_STATE));
			ZeroMemory(&m_CurrentState, sizeof(XINPUT_STATE));

			m_IsConnected = (XInputGetState(m_ControllerIndex, &m_CurrentState) == ERROR_SUCCESS);

			// XOR previous and current to find changed buttons
			const WORD buttonChanges = m_CurrentState.Gamepad.wButtons ^ m_PreviousState.Gamepad.wButtons;

			// Changed AND currently down = pressed this frame
			m_ButtonsPressedThisFrame = buttonChanges & m_CurrentState.Gamepad.wButtons;

			// Changed AND currently up = released this frame
			m_ButtonsReleasedThisFrame = buttonChanges & (~m_CurrentState.Gamepad.wButtons);
		}

		bool IsDown(unsigned int button)    const 
		{ 
			return m_ButtonsPressedThisFrame & button; 
		}

		bool IsUp(unsigned int button)      const 
		{
			return m_ButtonsReleasedThisFrame & button; 
		}

		bool IsPressed(unsigned int button) const 
		{ 
			return m_CurrentState.Gamepad.wButtons & button; 
		}

		bool IsConnected() const 
		{ 
			return m_IsConnected; 
		}

	private:
		unsigned int  m_ControllerIndex{};
		XINPUT_STATE  m_PreviousState{};
		XINPUT_STATE  m_CurrentState{};
		WORD          m_ButtonsPressedThisFrame{};
		WORD          m_ButtonsReleasedThisFrame{};
		bool          m_IsConnected{ false };
	};

	Controller::Controller(unsigned int controllerIndex)
		: m_pImpl(std::make_unique<ControllerImpl>(controllerIndex))
	{
	}

	Controller::~Controller() = default;

	void Controller::Update()
	{
		m_pImpl->Update();
	}

	bool Controller::IsDown(Button button) const
	{
		return m_pImpl->IsDown(static_cast<unsigned int>(button));
	}

	bool Controller::IsUp(Button button) const
	{
		return m_pImpl->IsUp(static_cast<unsigned int>(button));
	}

	bool Controller::IsPressed(Button button) const
	{
		return m_pImpl->IsPressed(static_cast<unsigned int>(button));
	}

	bool Controller::IsConnected() const
	{
		return m_pImpl->IsConnected();
	}
}