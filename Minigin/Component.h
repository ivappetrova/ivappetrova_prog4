#pragma once

namespace dae
{
	class GameObject;
	class Component
	{
	public:
		explicit Component(GameObject* owner) : m_owner(owner) {}
		virtual ~Component() = default;
		virtual void Update(float /*deltaTime*/) {}
		virtual void Render() const {}

		Component(const Component&) = delete;
		Component(Component&&) = delete;
		Component& operator=(const Component&) = delete;
		Component& operator=(const Component&&) = delete;

	protected:
		GameObject* m_owner;
	};
}