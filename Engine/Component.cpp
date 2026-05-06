#include "Component.h"
#include "GameObject.h"
#include "Texture2D.h"
#include "Renderer.h"

void dae::Component::RenderTexture(const Texture2D& texture) const
{
	const auto& POS = m_pOwner->GetWorldPosition();
	Renderer::GetInstance().RenderTexture(texture, POS.x, POS.y);
}