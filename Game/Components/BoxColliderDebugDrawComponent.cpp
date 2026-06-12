#include "Components/BoxColliderDebugDrawComponent.h"
#include "Components/BoxColliderComponent.h"
#include "Renderer.h"
#include "GameObject.h"
#include <SDL3/SDL.h>

namespace dae
{
	BoxColliderDebugDrawComponent::BoxColliderDebugDrawComponent(GameObject* owner, uint8_t r, uint8_t g, uint8_t b)
		: Component(owner), m_R(r), m_G(g), m_B(b) 
	{
	}

	void BoxColliderDebugDrawComponent::Render() const
	{
		auto* pBoxCollider{ GetOwner()->GetComponent<BoxColliderComponent>() };
		if (!pBoxCollider) return;

		SDL_Renderer* pRenderer = Renderer::GetInstance().GetSDLRenderer();
		SDL_SetRenderDrawColor(pRenderer, m_R, m_G, m_B, 255);

		const glm::vec2 TOP_LEFT { pBoxCollider->GetTopLeft() };
		const glm::vec2 TOP_RIGHT{ pBoxCollider->GetTopRight() };
		const glm::vec2 BOT_LEFT { pBoxCollider->GetBottomLeft()};
		const glm::vec2 BOT_RIGHT{ pBoxCollider->GetBottomRight() };

		SDL_RenderLine(pRenderer, TOP_LEFT.x,  TOP_LEFT.y,  TOP_RIGHT.x, TOP_RIGHT.y); // top
		SDL_RenderLine(pRenderer, BOT_LEFT.x,  BOT_LEFT.y,  BOT_RIGHT.x, BOT_RIGHT.y); // bottom
		SDL_RenderLine(pRenderer, TOP_LEFT.x,  TOP_LEFT.y,  BOT_LEFT.x,  BOT_LEFT.y);  // left
		SDL_RenderLine(pRenderer, TOP_RIGHT.x, TOP_RIGHT.y, BOT_RIGHT.x, BOT_RIGHT.y); // right

		SDL_SetRenderDrawColor(pRenderer, 255, 255, 255, 255);
	}
}