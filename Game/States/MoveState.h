#ifndef MOVE_STATE
#define MOVE_STATE

#include "PlayerState.h"

namespace dae
{
	class MoveState final : public PlayerState
	{
	public:
		void Enter(Player& player) override;
		PlayerState* HandleInput(Player& player) override;
		void Update(Player& player, float deltaTime) override;

	private:
		static constexpr float WALK_SPEED{ 80.f };
	};
}
#endif