#pragma once

namespace dae
{
	class GameObject;
	class Texture2D;
	class Component
	{
	public:
		explicit Component(GameObject* owner) : m_pOwner(owner) {}
		virtual ~Component() = default;
		virtual void Update(float /*deltaTime*/) {}
		virtual void Render() const {}
		virtual void FixedUpdate(float /*fixedDelta*/) {}

		Component(const Component&) = delete;
		Component(Component&&) = delete;
		Component& operator=(const Component&) = delete;
		Component& operator=(const Component&&) = delete;

	protected:
		GameObject* GetOwner() const { return m_pOwner; }
		void RenderTexture(const Texture2D& texture) const;

	private:
		GameObject* m_pOwner;
	};
}