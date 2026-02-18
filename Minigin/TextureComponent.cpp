#include "TextureComponent.h"
#include "ResourceManager.h"
#include "Renderer.h"
#include "Texture2D.h"
#include "GameObject.h"

dae::TextureComponent::TextureComponent(GameObject* owner)
	: Component(owner) {
}

void dae::TextureComponent::Render() const
{
	if (m_pTexture)
	{
		const auto& POS = m_pOwner->GetTransform().GetPosition();
		Renderer::GetInstance().RenderTexture(*m_pTexture, POS.x, POS.y);
	}
}

void dae::TextureComponent::SetTexture(const std::string& filename)
{
	m_pTexture = ResourceManager::GetInstance().LoadTexture(filename);
}