#include "TextureComponent.h"
#include "ResourceManager.h"
#include "Renderer.h"
#include "Texture2D.h"
#include "GameObject.h"
#include <SDL3/SDL.h>

dae::TextureComponent::TextureComponent(GameObject* owner, float width, float height)
	: Component(owner), m_Width(width), m_Height(height)
{}

void dae::TextureComponent::Render() const
{
	if (!m_pTexture) return;

	const SDL_FlipMode flip = FlipX ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
	SDL_Renderer* renderer = Renderer::GetInstance().GetSDLRenderer();

	if (m_Width > 0 && m_Height > 0)
	{
		const auto pos = GetOwner()->GetWorldPosition();
		SDL_FRect dst{ pos.x, pos.y - m_Height, m_Width, m_Height };
		SDL_RenderTextureRotated(renderer, m_pTexture->GetSDLTexture(), nullptr, &dst, 0.0, nullptr, flip);
	}
	else
	{
		const auto pos = GetOwner()->GetWorldPosition();
		const auto size = m_pTexture->GetSize();
		SDL_FRect dst{ pos.x, pos.y - size.y, size.x, size.y };
		SDL_RenderTextureRotated(renderer, m_pTexture->GetSDLTexture(), nullptr, &dst, 0.0, nullptr, flip);
	}
}

void dae::TextureComponent::SetTexture(const std::string& filename)
{
	m_pTexture = ResourceManager::GetInstance().LoadTexture(filename);
}

void dae::TextureComponent::SetTexture(std::shared_ptr<Texture2D> texture)
{
	m_pTexture = std::move(texture);
}