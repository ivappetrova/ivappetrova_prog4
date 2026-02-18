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
		const Transform& GetTransform() const { return m_transform; }

		template<typename T, typename... Args>
		T* AddComponent(Args&&... args)
		{
			static_assert(std::is_base_of_v<Component, T>, "T must derive from Component");
			auto& comp = m_components.emplace_back(
				std::make_unique<T>(this, std::forward<Args>(args)...)
			);
			return static_cast<T*>(comp.get());
		}

		template<typename T>
		T* GetComponent() const
		{
			for (auto& comp : m_components)
				if (auto* c = dynamic_cast<T*>(comp.get()))
					return c;
			return nullptr;
		}

		GameObject(const GameObject&) = delete;
		GameObject(GameObject&&) = delete;
		GameObject& operator=(const GameObject&) = delete;
		GameObject& operator=(GameObject&&) = delete;

	private:
		Transform m_transform{};
		std::vector<std::unique_ptr<Component>> m_components{};
	};
}
