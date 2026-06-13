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

	// unique ptr childten are automatically destroyed

	// or

	// keep world pos when the parent is destroyed
	//for (auto& child : m_pChildren)
	//{
	//	child->SetLocalPosition(child->GetWorldPosition());
	//	child->m_pParent = nullptr;
	//}

	// or

	// null out children's parent ptr before they get destroyed,
	// so their destructors don't try to erase from our already-dying vector
	/*for (auto& child : m_pChildren)
	{
		child->m_pParent = nullptr;
	}*/
	// m_pChildren vector destructor now safely destroys all children

}

void dae::GameObject::Update(float deltaTime)
{
	if (!m_IsActive) return;
	for (auto& comp : m_pComponents)
		comp->Update(deltaTime);
	for (auto& child : m_pChildren)
		child->Update(deltaTime);
}

void dae::GameObject::Render() const
{
	if (!m_IsActive) return;
	for (auto& comp : m_pComponents)
		comp->Render();
	for (auto& child : m_pChildren)
		child->Render();
}

void dae::GameObject::FixedUpdate(float fixedDelta)
{
	if (!m_IsActive) return;
	for (auto& comp : m_pComponents)
		comp->FixedUpdate(fixedDelta);
	for (auto& child : m_pChildren)
		child->FixedUpdate(fixedDelta);
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

const glm::vec3& dae::GameObject::GetWorldPosition()
{
	if (m_Transform.IsDirty())
	{
		UpdateWorldPosition();
	}
	return m_Transform.GetWorldPosition();
}

void dae::GameObject::UpdateWorldPosition()
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