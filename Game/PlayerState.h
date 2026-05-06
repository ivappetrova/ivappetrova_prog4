#pragma once

namespace dae
{
	class Player;

	class PlayerState
	{
	public:
		virtual ~PlayerState() = default;

		virtual void Enter(Player& /*player*/) {}
		virtual PlayerState* HandleInput(Player& player) = 0;
		virtual void Update(Player& player, float deltaTime) = 0;
		virtual void Exit(Player& /*player*/) {}

		PlayerState(const PlayerState&) = delete;
		PlayerState(PlayerState&&) = delete;
		PlayerState& operator=(const PlayerState&) = delete;
		PlayerState& operator=(PlayerState&&) = delete;

	protected:
		PlayerState() = default;
	};

} 