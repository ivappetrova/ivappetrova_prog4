#include "LevelCollisionComponent.h"
#include "SVGParser.h"
#include <cfloat>
#include <cmath>
#include <glm/glm.hpp>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <string>

namespace dae
{
	LevelCollisionComponent::LevelCollisionComponent(GameObject* owner)
		: Component(owner) 
	{
	}

	static glm::vec2 ReadSVGViewBox(const std::string& path)
	{
		std::ifstream file(path);
		if (!file) return { 0, 0 };

		std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

		const std::string KEY = "viewBox=\"";
		const size_t POS = content.find(KEY);
		if (POS == std::string::npos) return { 0, 0 };

		std::istringstream ss(content.substr(POS + KEY.size()));
		float x{};
		float y{};
		float w{};
		float h{};
		ss >> x >> y >> w >> h;
		return { w, h };
	}

	bool LevelCollisionComponent::LoadFromSVG(const std::string& path, float targetWidth, float targetHeight)
	{
		m_Polygons.clear();
		if (!SVGParser::GetVerticesFromSvgFile(path, m_Polygons)) return false;

		if (targetWidth > 0.f && targetHeight > 0.f)
		{
			const glm::vec2 SVG_SIZE = ReadSVGViewBox(path);
			if (SVG_SIZE.x > 0.f && SVG_SIZE.y > 0.f)
			{
				const float SCALE_X = targetWidth / SVG_SIZE.x;
				const float SCALE_Y = targetHeight / SVG_SIZE.y;
				for (auto& poly : m_Polygons)
				{ 
					for (auto& p : poly)
					{
						p.x *= SCALE_X;
						p.y *= SCALE_Y;
					}
				}
			}
		}

		return true;
	}

	void LevelCollisionComponent::SetPolygons(std::vector<std::vector<glm::vec2>> polygons)
	{
		m_Polygons = std::move(polygons);
	}

	bool LevelCollisionComponent::Overlaps(const glm::vec2& point) const
	{
		for (const auto& poly : m_Polygons)
		{
			if (PointInPolygon(point, poly)) return true;
		}
		return false;
	}

	bool LevelCollisionComponent::IsGrounded(const glm::vec2& point, float tolerance) const
	{
		for (const auto& POLY : m_Polygons)
		{
			const size_t POLYGONS_AMOUNT = POLY.size();
			for (size_t index{}; index < POLYGONS_AMOUNT; ++index)
			{
				const glm::vec2& a = POLY[index];
				const glm::vec2& b = POLY[(index + 1) % POLYGONS_AMOUNT];
				const float dy = std::abs(b.y - a.y);
				const float dx = std::abs(b.x - a.x);
				if (dx < 0.001f || dy / dx > 0.3f) continue;
				if (PointToSegmentDist(point, a, b) <= tolerance)
				{
					return true;
				}
			}
		}
		return false;
	}

	float LevelCollisionComponent::GroundYBelow(const glm::vec2& point, float maxDist) const
	{
		float closest = FLT_MAX;
		for (const auto& POLY : m_Polygons)
		{
			const size_t POLYGONS_AMOUNT = POLY.size();
			for (size_t index{}; index < POLYGONS_AMOUNT; ++index)
			{
				const glm::vec2& a = POLY[index];
				const glm::vec2& b = POLY[(index + 1) % POLYGONS_AMOUNT];
				const float dy = std::abs(b.y - a.y);
				const float dx = std::abs(b.x - a.x);
				if (dx < 0.001f || dy / dx > 0.3f) continue;
				const float minX = std::min(a.x, b.x);
				const float maxX = std::max(a.x, b.x);
				if (point.x < minX || point.x > maxX) continue;
				const float t = (point.x - a.x) / (b.x - a.x);
				const float edgeY = a.y + t * (b.y - a.y);
				if (edgeY >= point.y && edgeY - point.y <= maxDist)
				{
					closest = std::min(closest, edgeY);
				}
			}
		}
		return closest;
	}

	bool LevelCollisionComponent::PointInPolygon(const glm::vec2& p, const std::vector<glm::vec2>& poly)
	{
		bool inside{ false };
		const size_t POLYGONS_AMOUNT = poly.size();
		for (size_t index{}, j = POLYGONS_AMOUNT - 1; index < POLYGONS_AMOUNT; j = index++)
		{
			const glm::vec2& pi = poly[index];
			const glm::vec2& pj = poly[j];
			if (((pi.y > p.y) != (pj.y > p.y)) &&
				(p.x < (pj.x - pi.x) * (p.y - pi.y) / (pj.y - pi.y) + pi.x))
			{
				inside = !inside;
			}
		}
		return inside;
	}

	float LevelCollisionComponent::PointToSegmentDist(const glm::vec2& p, const glm::vec2& a, const glm::vec2& b)
	{
		const glm::vec2 ab = b - a;
		const glm::vec2 ap = p - a;
		const float lenSq = ab.x * ab.x + ab.y * ab.y;
		if (lenSq < 0.0001f) return glm::length(ap);
		const float t = std::clamp((ap.x * ab.x + ap.y * ab.y) / lenSq, 0.f, 1.f);
		return glm::length(p - (a + t * ab));
	}

float LevelCollisionComponent::WallXBeside(const glm::vec2& point, float maxDist, bool movingRight) const
	{
		const glm::vec2 moveDir{ movingRight ? 1.f : -1.f, 0.f };
		float best = movingRight ? FLT_MAX : -FLT_MAX;

		for (const auto& poly : m_Polygons)
		{
			const size_t n = poly.size();
			for (size_t i = 0; i < n; ++i)
			{
				const glm::vec2& a = poly[i];
				const glm::vec2& b = poly[(i + 1) % n];

				const glm::vec2 edge{ b.x - a.x, b.y - a.y };
				const glm::vec2 normal{ -edge.y, edge.x };  // left-hand normal

				if (normal.x * moveDir.x + normal.y * moveDir.y >= 0.f) continue;

				const float dx = std::abs(edge.x);
				const float dy = std::abs(edge.y);
				if (dy < 0.001f) continue;
				if (dx > 0.001f && dy / dx <= 0.3f) continue;

				const float lenSq = edge.x * edge.x + edge.y * edge.y;
				const glm::vec2 ap{ point.x - a.x, point.y - a.y };
				const float t = std::clamp((ap.x * edge.x + ap.y * edge.y) / lenSq, 0.f, 1.f);
				const glm::vec2 closest{ a.x + t * edge.x, a.y + t * edge.y };

				const float xDist = std::abs(closest.x - point.x);
				if (xDist > maxDist) continue;

				const float yDist = std::abs(closest.y - point.y);
				if (yDist > maxDist) continue;

				if (movingRight)
				{
					if (closest.x >= point.x)
					{
						best = std::min(best, closest.x);
					}
				}
				else
				{
					if (closest.x <= point.x)

						best = std::max(best, closest.x);
				}
			}
		}
		return best;
	}
}