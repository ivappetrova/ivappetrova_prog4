#pragma once

namespace dae
{
	class DeltaTime final
	{
	public:
		static float Get() { return m_DeltaTime; }

	private:
		friend class Minigin;
		static void Set(float dt) { m_DeltaTime = dt; }

		static inline float m_DeltaTime{ 0.f };
	};
}