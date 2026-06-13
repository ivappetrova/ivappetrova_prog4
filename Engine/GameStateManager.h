#ifndef GAMESTATEMANAGER_H
#define GAMESTATEMANAGER_H

#include <memory>
#include "IGameState.h"

namespace dae
{
	class GameStateManager final
	{
	public:
		GameStateManager() = default;
		~GameStateManager() { if (m_pCurrent) m_pCurrent->Exit(); }

		// Rule of 5
		GameStateManager(const GameStateManager&) = delete;
		GameStateManager(GameStateManager&&) = delete;
		GameStateManager& operator=(const GameStateManager&) = delete;
		GameStateManager& operator=(GameStateManager&&) = delete;

		void SwitchTo(std::unique_ptr<IGameState> newState)
		{
			m_pPending = std::move(newState);
		}

		void Update(float deltaTime)
		{
			if (m_pPending)
			{
				if (m_pCurrent)
				{
					m_pCurrent->Exit();
				}
				m_pCurrent = std::move(m_pPending);
				m_pCurrent->Enter();
			}
			if (m_pCurrent)
			{
				m_pCurrent->Update(deltaTime);
			}
		}

		void Render() const
		{
			if (m_pCurrent)
			{
				m_pCurrent->Render();
			}
		}

		bool HasState() const { return m_pCurrent != nullptr; }

		void FlushPendingTransition()
		{
			if (!m_pPending) return;
			if (m_pCurrent)
			{
				m_pCurrent->Exit();
			}
			m_pCurrent = std::move(m_pPending);
			m_pCurrent->Enter();
		}

	private:
		std::unique_ptr<IGameState> m_pCurrent;
		std::unique_ptr<IGameState> m_pPending;
	};
}
#endif