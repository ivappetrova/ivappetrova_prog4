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
		RenderTexture(*m_pTexture);
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