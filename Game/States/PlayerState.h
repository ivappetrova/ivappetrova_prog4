#ifndef PLAYERSTATE_H
#define PLAYERSTATE_H

namespace dae
{
	class PlayerComponent;

	class PlayerState
	{
	public:
		virtual ~PlayerState() = default;

		virtual void Enter(PlayerComponent& /*PlayerComponent*/) {}
		virtual PlayerState* HandleInput(PlayerComponent& PlayerComponent) = 0;
		virtual void Update(PlayerComponent& PlayerComponent, float deltaTime) = 0;
		virtual void Exit(PlayerComponent& /*PlayerComponent*/) {}

		// Rule of 5
		PlayerState(const PlayerState&) = delete;
		PlayerState(PlayerState&&) = delete;
		PlayerState& operator=(const PlayerState&) = delete;
		PlayerState& operator=(PlayerState&&) = delete;

	protected:
		PlayerState() = default;
	};
}
#endif