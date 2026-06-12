#include "LevelDebugDrawComponent.h"
#include "LevelCollisionComponent.h"
#include "Renderer.h"          // dae::Renderer::GetInstance().GetSDLRenderer()
#include <SDL3/SDL.h>

namespace dae
{
	LevelDebugDrawComponent::LevelDebugDrawComponent(GameObject* owner, LevelCollisionComponent* collision)
		: Component(owner), m_pCollision(collision) 
	{
}

	void LevelDebugDrawComponent::Render() const
	{
		if (!m_pCollision) return;

		SDL_Renderer* pRenderer = Renderer::GetInstance().GetSDLRenderer();
		SDL_SetRenderDrawColor(pRenderer, 0, 255, 0, 255);

		for (const auto& POLYGONS : m_pCollision->GetPolygons())
		{
			const size_t POLYGONS_AMOUNT = POLYGONS.size();
			if (POLYGONS_AMOUNT < 2) continue;
			for (size_t index{}; index < POLYGONS_AMOUNT; ++index)
			{
				const auto& a = POLYGONS[index];
				const auto& b = POLYGONS[(index + 1) % POLYGONS_AMOUNT];
				SDL_RenderLine(pRenderer, a.x, a.y, b.x, b.y);
			}
		}

		// Restore white so other render calls aren't tinted
		SDL_SetRenderDrawColor(pRenderer, 255, 255, 255, 255);
	}
}