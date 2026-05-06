#pragma once
#include <vector>
#include <memory>
#include <type_traits>
#include <glm/glm.hpp>
#include "Transform.h"

namespace dae
{
	class Component;

	class GameObject final
	{
	public:
		GameObject();
		~GameObject();

		void Update(float deltaTime);
		void Render() const;
		void FixedUpdate(float fixedDelta);

		// Local position — marks dirty
		void SetLocalPosition(float x, float y);
		void SetLocalPosition(const glm::vec3& pos);
		// World position — recalculates lazily if dirty
		const glm::vec3& GetWorldPosition() const;

		const Transform& GetTransform() const { return m_Transform; }

		// Parent is non-owning: the scene/parent owns children, not the other way around
		void SetParent(GameObject* parent, bool keepWorldPosition = false);
		GameObject* GetParent() const { return m_pParent; }
		size_t GetChildCount() const { return m_pChildren.size(); }
		GameObject* GetChildAt(size_t index) const { return m_pChildren[index].get(); }

		void MarkForDestroy() { m_IsMarkedForDestroy = true; }
		bool IsMarkedForDestroy() const { return m_IsMarkedForDestroy; }

		template<typename T, typename... Args>
		T* AddComponent(Args&&... args)
		{
			static_assert(std::is_base_of_v<Component, T>, "T must derive from Component");
			auto& comp = m_pComponents.emplace_back(std::make_unique<T>(this, std::forward<Args>(args)...));
			return static_cast<T*>(comp.get());
		}

		template<typename T>
		T* GetComponent() const
		{
			for (auto& componentPtr : m_pComponents)
				if (auto* component = dynamic_cast<T*>(componentPtr.get()))
					return component;
			return nullptr;
		}

		template<typename T>
		bool HasComponent() const { return GetComponent<T>() != nullptr; }

		template<typename T>
		void RemoveComponent()
		{
			m_pComponents.erase(
				std::remove_if(m_pComponents.begin(), m_pComponents.end(),
					[](const std::unique_ptr<Component>& c) { return dynamic_cast<T*>(c.get()) != nullptr; }),
				m_pComponents.end());
		}

		GameObject(const GameObject&) = delete;
		GameObject(GameObject&&) = delete;
		GameObject& operator=(const GameObject&) = delete;
		GameObject& operator=(GameObject&&) = delete;

	private:
		bool IsChild(const GameObject* candidate) const;
		// Takes ownership of the child
		void AddChild(std::unique_ptr<GameObject> child);
		// Releases ownership and returns the child
		std::unique_ptr<GameObject> RemoveChild(GameObject* child);

		void UpdateWorldPosition() const;
		void SetPositionDirty();

		Transform m_Transform{};
		std::vector<std::unique_ptr<Component>> m_pComponents{};
		bool m_IsMarkedForDestroy{ false };

		// Non-owning: parent owns this object (via m_pChildren)
		GameObject* m_pParent{ nullptr };
		// Owning: this object owns its children
		std::vector<std::unique_ptr<GameObject>> m_pChildren{};

	};
}