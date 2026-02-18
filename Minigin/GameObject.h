#pragma once
#include <vector>
#include <memory>
#include <type_traits>
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

		void SetPosition(float x, float y);
		const Transform& GetTransform() const { return m_Transform; }

		template<typename T, typename... Args>
		T* AddComponent(Args&&... args)
		{
			static_assert(std::is_base_of_v<Component, T>, "T must derive from Component");
			auto& comp = m_pComponents.emplace_back( std::make_unique<T>(this, std::forward<Args>(args)...));
			return static_cast<T*>(comp.get());
		}

		template<typename T>
		T* GetComponent() const
		{
			for (auto& componentPtr : m_pComponents)
			{
				if (auto* component = dynamic_cast<T*>(componentPtr.get()))
				{
					return component;
				}
			}
				return nullptr;
		}

		template<typename T>
		bool HasComponent() const
		{
			return GetComponent<T>() != nullptr;
		}
		
		template<typename T>
		void RemoveComponent()
		{
			m_pComponents.erase(
				std::remove_if(m_pComponents.begin(), m_pComponents.end(),
					[](const std::unique_ptr<Component>& componentPtr)
					{
						return dynamic_cast<T*>(componentPtr.get()) != nullptr;
					}),
				m_pComponents.end());
		}

		void FixedUpdate(float fixedDelta);

		GameObject(const GameObject&) = delete;
		GameObject(GameObject&&) = delete;
		GameObject& operator=(const GameObject&) = delete;
		GameObject& operator=(GameObject&&) = delete;

	private:
		Transform m_Transform{};
		std::vector<std::unique_ptr<Component>> m_pComponents{};
	};
}
