#ifndef IDLE_STATE
#define IDLE_STATE

#include "PlayerState.h"

namespace dae
{
	class IdleState final : public PlayerState
	{
	public:
		void Enter(Player& player) override;
		PlayerState* HandleInput(Player& player) override;
		void Update(Player& player, float deltaTime) override;
	};
}
#endif