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
		explicit TextureComponent(GameObject* owner);
		void Render() const override;
		void SetTexture(const std::string& filename);
		void SetTexture(std::shared_ptr<Texture2D> texture);
	private:
		std::shared_ptr<Texture2D> m_pTexture{};
	};
}
#endif