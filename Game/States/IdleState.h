#ifndef IDLE_STATE
#define IDLE_STATE

#include "PlayerState.h"

namespace dae
{
	class IdleState final : public PlayerState
	{
	public:
		void Enter(PlayerComponent& PlayerComponent) override;
		PlayerState* HandleInput(PlayerComponent& PlayerComponent) override;
		void Update(PlayerComponent& PlayerComponent, float deltaTime) override;
	};
}
#endif