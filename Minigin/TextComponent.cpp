#include <stdexcept>
#include <SDL3_ttf/SDL_ttf.h>
#include "TextComponent.h"
#include "Renderer.h"
#include "Font.h"
#include "Texture2D.h"
#include "GameObject.h"

dae::TextComponent::TextComponent(GameObject* owner, const std::string& text,
	std::shared_ptr<Font> font, const SDL_Color& color)
	: Component(owner), m_Text(text), m_Color(color), m_pFont(std::move(font)) 
{
}

void dae::TextComponent::Update(float)
{
	if (m_NeedsUpdate)
	{
		const auto SURFACE = TTF_RenderText_Blended(m_pFont->GetFont(), m_Text.c_str(), m_Text.length(), m_Color);
		if (!SURFACE)
		{
			throw std::runtime_error(std::string("Render text failed: ") + SDL_GetError());
		}

		auto texture = SDL_CreateTextureFromSurface(Renderer::GetInstance().GetSDLRenderer(), SURFACE);
		if (!texture)
		{
			throw std::runtime_error(std::string("Create text texture from surface failed: ") + SDL_GetError());
		}

		SDL_DestroySurface(SURFACE);
		m_pTextTexture = std::make_shared<Texture2D>(texture);
		m_NeedsUpdate = false;
	}
}

void dae::TextComponent::Render() const
{
	if (m_pTextTexture)
	{
		RenderTexture(*m_pTextTexture);
	}
}

void dae::TextComponent::SetText(const std::string& text)
{
	m_Text = text;
	m_NeedsUpdate = true;
}

void dae::TextComponent::SetColor(const SDL_Color& color)
{
	m_Color = color;
	m_NeedsUpdate = true;
}