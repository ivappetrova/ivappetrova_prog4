#include "GameObject.h"
#include "Component.h"

dae::GameObject::GameObject() = default;
dae::GameObject::~GameObject() = default;

void dae::GameObject::Update(float deltaTime)
{
	for (auto& comp : m_pComponents)
		comp->Update(deltaTime);
}

void dae::GameObject::Render() const
{
	for (auto& comp : m_pComponents)
		comp->Render();
}

void dae::GameObject::SetPosition(float x, float y)
{
	m_Transform.SetPosition(x, y, 0.f);
}

void dae::GameObject::FixedUpdate(float fixedDelta)
{
	for (auto& component : m_pComponents)
	{
		component->FixedUpdate(fixedDelta);
	}
}