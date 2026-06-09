#ifndef JUMP_STATE
#define JUMP_STATE

#include "PlayerState.h"

namespace dae
{
	class JumpState final : public PlayerState
	{
	public:
		void Enter(Player& player) override;
		PlayerState* HandleInput(Player& player) override;
		void Update(Player& player, float deltaTime) override;
	};
}
#endif