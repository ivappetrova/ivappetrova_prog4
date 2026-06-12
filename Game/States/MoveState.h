#ifndef MOVE_STATE
#define MOVE_STATE

#include "PlayerState.h"

namespace dae
{
	class MoveState final : public PlayerState
	{
	public:
		void Enter(PlayerComponent& PlayerComponent) override;
		PlayerState* HandleInput(PlayerComponent& PlayerComponent) override;
		void Update(PlayerComponent& PlayerComponent, float deltaTime) override;
		void Exit(PlayerComponent& PlayerComponent) override;
	};
}
#endif