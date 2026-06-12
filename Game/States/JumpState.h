#ifndef JUMP_STATE
#define JUMP_STATE

#include "PlayerState.h"

namespace dae
{
	class JumpState final : public PlayerState
	{
	public:
		void Enter(PlayerComponent& PlayerComponent) override;
		PlayerState* HandleInput(PlayerComponent& PlayerComponent) override;
		void Update(PlayerComponent& PlayerComponent, float deltaTime) override;
	};
}
#endif