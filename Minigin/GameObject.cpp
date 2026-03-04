#include "GameObject.h"
#include "Component.h"
#include <algorithm>

dae::GameObject::GameObject() = default;

dae::GameObject::~GameObject()
{
	if (m_pParent)
	{
		m_pParent->RemoveChild(this);
	}

	for (auto* child : m_pChildren)
	{
		child->m_pParent = nullptr;
		delete child;
	}
}

void dae::GameObject::Update(float deltaTime)
{
	for (auto& comp : m_pComponents)
	{
		comp->Update(deltaTime);
	}

	for (auto* child : m_pChildren)
	{
		child->Update(deltaTime);
	}
}

void dae::GameObject::Render() const
{
	for (auto& comp : m_pComponents)
	{
		comp->Render();
	}

	for (auto* child : m_pChildren)
	{
		child->Render();
	}
}

void dae::GameObject::FixedUpdate(float fixedDelta)
{
	for (auto& comp : m_pComponents)
	{
		comp->FixedUpdate(fixedDelta);
	}

	for (auto* child : m_pChildren)
	{
		child->FixedUpdate(fixedDelta);
	}
}

void dae::GameObject::SetLocalPosition(float x, float y)
{
	SetLocalPosition(glm::vec3{ x, y, 0.f });
}

void dae::GameObject::SetLocalPosition(const glm::vec3& pos)
{
	m_Transform.SetLocalPosition(pos);
	SetPositionDirty();
}

const glm::vec3& dae::GameObject::GetWorldPosition() const
{
	if (m_Transform.IsDirty())
	{
		UpdateWorldPosition();
	}
	return m_Transform.GetWorldPosition();
}

void dae::GameObject::UpdateWorldPosition() const
{
	if (m_pParent == nullptr)
	{
		m_Transform.SetWorldPosition(m_Transform.GetLocalPosition());
	}
	else
	{
		m_Transform.SetWorldPosition(m_pParent->GetWorldPosition() + m_Transform.GetLocalPosition());
	}
}

void dae::GameObject::SetParent(GameObject* parent, bool keepWorldPosition)
{
	if (IsChild(parent) || parent == this || m_pParent == parent)
	{
		return;
	}

	if (parent == nullptr)
	{
		SetLocalPosition(GetWorldPosition());
	}
	else
	{
		if (keepWorldPosition)
		{
			SetLocalPosition(GetWorldPosition() - parent->GetWorldPosition());
		}
		SetPositionDirty();
	}

	if (m_pParent)
	{
		m_pParent->RemoveChild(this);
	}

	m_pParent = parent;

	if (m_pParent)
	{
		m_pParent->AddChild(this);
	}
}

bool dae::GameObject::IsChild(const GameObject* candidate) const
{
	for (const auto* child : m_pChildren)
	{
		if (child == candidate) return true;
		if (child->IsChild(candidate)) return true;
	}
	return false;
}

void dae::GameObject::AddChild(GameObject* child)
{
	m_pChildren.push_back(child);
}

void dae::GameObject::RemoveChild(GameObject* child)
{
	m_pChildren.erase(
		std::remove(m_pChildren.begin(), m_pChildren.end(), child),
		m_pChildren.end());
}

void dae::GameObject::SetPositionDirty()
{
	m_Transform.SetDirty();

	for (auto* child : m_pChildren)
	{
		child->SetPositionDirty();
	}
}