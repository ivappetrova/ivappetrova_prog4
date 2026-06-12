#ifndef TEXTURECOMPONENT_H
#define TEXTURECOMPONENT_H

#include <memory>
#include <string>

#include "Component.h"

namespace dae
{
	class Texture2D;
	class TextureComponent final : public Component
	{
	public:
		explicit TextureComponent(GameObject* owner, float width = 0.f, float height = 0.f);
		void Render() const override;
		void SetTexture(const std::string& filename);
		void SetTexture(std::shared_ptr<Texture2D> texture);

		float GetWidth()  const { return m_Width; }
		float GetHeight() const { return m_Height; }

		bool FlipX{ false };

	private:
		std::shared_ptr<Texture2D> m_pTexture{};
		float m_Width{};
		float m_Height{};
	};
}
#endif