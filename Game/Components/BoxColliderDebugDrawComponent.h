#ifndef BOXCOLLIDERDEBUGDRAW_H
#define BOXCOLLIDERDEBUGDRAW_H

#include <cstdint>

#include "Component.h"

namespace dae
{
	class BoxColliderComponent;

	class BoxColliderDebugDrawComponent final : public Component
	{
	public:
		BoxColliderDebugDrawComponent(GameObject* owner, uint8_t r = 0, uint8_t g = 255, uint8_t b = 255);
		void Render() const override;

	private:
		uint8_t m_R;
		uint8_t m_G;
		uint8_t m_B;
	};
}
#endif