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
	if (m_texture)
	{
		const auto& pos = m_owner->GetTransform().GetPosition();
		Renderer::GetInstance().RenderTexture(*m_texture, pos.x, pos.y);
	}
}

void dae::TextureComponent::SetTexture(const std::string& filename)
{
	m_texture = ResourceManager::GetInstance().LoadTexture(filename);
}