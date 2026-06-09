#ifndef SHOOT_STATE
#define SHOOT_STATE

#include "PlayerState.h"

namespace dae
{
	class ShootState final : public PlayerState
	{
	public:
		explicit ShootState(bool wasMoving = false);

		void Enter(Player& player) override;
		PlayerState* HandleInput(Player& player) override;
		void Update(Player& player, float deltaTime) override;

	private:
		float m_Timer{ 0.f };
		bool  m_WasMoving{ false };
		static constexpr float SHOOT_DURATION{ 0.3f };
	};
}
#endif