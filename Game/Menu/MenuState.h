#ifndef MENU_STATE_H
#define MENU_STATE_H

#include "IGameState.h"
#include "SoundSystem.h"

namespace dae
{
	class GameStateManager;
	class GameObject;

	class MenuState final : public IGameState
	{
	public:
		MenuState(GameStateManager& gsm, float windowWidth, float windowHeight);
		~MenuState() override = default;

		void Enter()  override;
		void Exit()   override;
		void Update(float deltaTime) override;
		void Render() const override;

		void Navigate(int delta);
		void Confirm();

	private:
		void UpdateSelectorPosition();

		static constexpr int NUM_OPTIONS = 3;

		GameStateManager& m_GSM;
		float m_WindowWidth;
		float m_WindowHeight;

		int m_Selected{ 0 };
		GameObject* m_pSelector{ nullptr };
		sound_id m_MusicId{};
	};
}
#endif