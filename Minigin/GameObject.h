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

		// Hierarchy — SetParent is the primary interface (slide 16)
		// keepWorldPosition: if true, local position adjusts so world position stays the same
		void SetParent(GameObject* parent, bool keepWorldPosition = false);
		GameObject* GetParent() const { return m_pParent; }
		size_t GetChildCount() const { return m_pChildren.size(); }
		GameObject* GetChildAt(size_t index) const { return m_pChildren[index]; }

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

		void AddChild(GameObject* child);
		void RemoveChild(GameObject* child);

		void UpdateWorldPosition() const;
		void SetPositionDirty();

		Transform m_Transform{};
		std::vector<std::unique_ptr<Component>> m_pComponents{};
		bool m_IsMarkedForDestroy{ false };

		GameObject* m_pParent{ nullptr };
		std::vector<GameObject*> m_pChildren{}; 
	};
}