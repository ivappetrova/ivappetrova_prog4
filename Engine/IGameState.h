#ifndef IGAMESTATE_H
#define IGAMESTATE_H

namespace dae
{
	class IGameState
	{
	public:
		virtual ~IGameState() = default;

		virtual void Enter() = 0;
		virtual void Exit() = 0;
		virtual void Update(float deltaTime) = 0;
		virtual void Render() const {}
	};
}
#endif