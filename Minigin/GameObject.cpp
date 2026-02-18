#include "GameObject.h"
#include "Component.h"

dae::GameObject::GameObject() = default;
dae::GameObject::~GameObject() = default;

void dae::GameObject::Update(float deltaTime)
{
	for (auto& comp : m_components)
		comp->Update(deltaTime);
}

void dae::GameObject::Render() const
{
	for (auto& comp : m_components)
		comp->Render();
}

void dae::GameObject::SetPosition(float x, float y)
{
	m_transform.SetPosition(x, y, 0.f);
}
