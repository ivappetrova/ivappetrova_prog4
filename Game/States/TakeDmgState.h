#ifndef TAKE_DMG_STATE
#define TAKE_DMG_STATE

#include "PlayerState.h"

namespace dae
{
	class TakeDmgState final : public PlayerState
	{
	public:
		void Enter(Player& player) override;
		PlayerState* HandleInput(Player& player) override;
		void Update(Player& player, float deltaTime) override;

	private:
		float m_Timer{ 0.5f };
		static constexpr float KNOCKBACK_VX{ -150.f };
	};
}
#endif