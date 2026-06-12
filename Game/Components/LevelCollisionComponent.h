#ifndef LEVELCOLLISIONCOMPONENT_H
#define LEVELCOLLISIONCOMPONENT_H

#include "Component.h"
#include <vector>
#include <string>
#include <glm/vec2.hpp>

namespace dae
{
	class LevelCollisionComponent final : public Component
	{
	public:
		explicit LevelCollisionComponent(GameObject* owner);

		bool LoadFromSVG(const std::string& path, float targetWidth = 0.f, float targetHeight = 0.f);
		void SetPolygons(std::vector<std::vector<glm::vec2>> polygons);

		bool Overlaps(const glm::vec2& point) const;
		bool IsGrounded(const glm::vec2& point, float tolerance = 3.f) const;
		float GroundYBelow(const glm::vec2& point, float maxDist = 512.f) const;
		float WallXBeside(const glm::vec2& point, float maxDist, bool movingRight) const;

		const std::vector<std::vector<glm::vec2>>& GetPolygons() const { return m_Polygons; }

	private:
		static bool PointInPolygon(const glm::vec2& p, const std::vector<glm::vec2>& poly);
		static float PointToSegmentDist(const glm::vec2& p, const glm::vec2& a, const glm::vec2& b);

		std::vector<std::vector<glm::vec2>> m_Polygons;
	};
}
#endif