#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <glm/glm.hpp>

namespace dae
{
	class Transform final
	{
	public:
		const glm::vec3& GetLocalPosition() const { return m_LocalPosition; }
		const glm::vec3& GetWorldPosition() const { return m_WorldPosition; }

		void SetLocalPosition(const glm::vec3& position);
		void SetWorldPosition(const glm::vec3& pos);

		bool IsDirty() const { return m_IsDirty; }
		void SetDirty() { m_IsDirty = true; }

	private:
		glm::vec3 m_LocalPosition{};
		glm::vec3 m_WorldPosition{}; 
		bool m_IsDirty{ true };      
	};
}
#endif