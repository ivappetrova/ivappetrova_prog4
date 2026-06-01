#ifndef TEXTCOMPONENT_H
#define TEXTCOMPONENT_H

#include <string>
#include <memory>
#include <SDL3/SDL.h>

#include "Component.h"

namespace dae
{
	class Font;
	class TextureComponent;

	class TextComponent final : public Component
	{
	public:
		TextComponent(GameObject* owner, const std::string& text, std::shared_ptr<Font> font, const SDL_Color& color = { 255, 255, 255, 255 });
		void Update(float deltaTime) override;
		void SetText(const std::string& text);
		void SetColor(const SDL_Color& color);
	private:
		bool m_NeedsUpdate{ true };
		std::string m_Text{};
		SDL_Color m_Color{};
		std::shared_ptr<Font> m_pFont{};

		TextureComponent* m_pTextureComponent{ nullptr };
	};
}
#endif