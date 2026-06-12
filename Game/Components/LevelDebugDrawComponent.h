#ifndef LEVELDEBUGDRAWCOMPONENT_H
#define LEVELDEBUGDRAWCOMPONENT_H

#include "Component.h"

namespace dae
{
	class LevelCollisionComponent;

	class LevelDebugDrawComponent final : public Component
	{
	public:
		LevelDebugDrawComponent(GameObject* owner, LevelCollisionComponent* collision);
		void Render() const override;

	private:
		LevelCollisionComponent* m_pCollision;
	};
}
#endif