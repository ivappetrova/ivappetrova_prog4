#include "GameObject.h"
#include "Component.h"
#include <algorithm>

dae::GameObject::GameObject() = default;

dae::GameObject::~GameObject()
{
	if (m_pParent)
	{
		auto& siblings = m_pParent->m_pChildren;
		siblings.erase(
			std::remove_if(siblings.begin(), siblings.end(),
				[this](const std::unique_ptr<GameObject>& c) { return c.get() == this; }),
			siblings.end());
	}

	for (auto& child : m_pChildren)
	{
		child->m_pParent = nullptr;
	}
}

void dae::GameObject::Update(float deltaTime)
{
	for (auto& comp : m_pComponents)
	{
		comp->Update(deltaTime);
	}

	for (auto& child : m_pChildren)
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

	for (auto& child : m_pChildren)
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

	for (auto& child : m_pChildren)
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

void dae::GameObject::SetParent(GameObject* newParent, bool keepWorldPosition)
{
	if (IsChild(newParent) || newParent == this || m_pParent == newParent)
		return;

	if (newParent == nullptr)
		SetLocalPosition(GetWorldPosition());
	else if (keepWorldPosition)
		SetLocalPosition(GetWorldPosition() - newParent->GetWorldPosition());

	if (m_pParent)
	{
		std::unique_ptr<GameObject> self = m_pParent->RemoveChild(this);
		m_pParent = newParent;
		if (m_pParent)
		{
			m_pParent->AddChild(std::move(self));
		}
	}
	else
	{
		m_pParent = newParent;
	}

	SetPositionDirty();
}

bool dae::GameObject::IsChild(const GameObject* candidate) const
{
	for (const auto& child : m_pChildren)
	{
		if (child.get() == candidate) return true;
		if (child->IsChild(candidate)) return true;
	}
	return false;
}

void dae::GameObject::AddChild(std::unique_ptr<GameObject> child)
{
	m_pChildren.push_back(std::move(child));
}

std::unique_ptr<dae::GameObject> dae::GameObject::RemoveChild(GameObject* child)
{
	auto it = std::find_if(m_pChildren.begin(), m_pChildren.end(),
		[child](const std::unique_ptr<GameObject>& c) { return c.get() == child; });

	if (it == m_pChildren.end())
	{
		return nullptr;
	}

	std::unique_ptr<GameObject> owned = std::move(*it);
	m_pChildren.erase(it);
	owned->m_pParent = nullptr;
	return owned;
}

void dae::GameObject::SetPositionDirty()
{
	m_Transform.SetDirty();
	for (auto& child : m_pChildren)
	{
		child->SetPositionDirty();
	}
}