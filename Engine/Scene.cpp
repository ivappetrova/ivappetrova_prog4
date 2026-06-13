#include <algorithm>
#include <vector>
#include "Scene.h"

using namespace dae;

void Scene::Add(std::unique_ptr<GameObject> object)
{
	assert(object != nullptr && "Cannot add a null GameObject to the scene.");
	m_pObjects.emplace_back(std::move(object));
}

void Scene::Remove(const GameObject& object)
{
	m_pObjects.erase(std::remove_if(m_pObjects.begin(), m_pObjects.end(), [&object](const auto& ptr) { return ptr.get() == &object; }), m_pObjects.end());
}

void Scene::RemoveAll()
{
	m_pObjects.clear();
}

void Scene::Update(float deltaTime)
{
	for (auto& objectPtr : m_pObjects)
	{
		objectPtr->Update(deltaTime);
	}

	m_pObjects.erase(
		std::remove_if(m_pObjects.begin(), m_pObjects.end(),
			[](const auto& ptr) { return ptr->IsMarkedForDestroy(); }),
		m_pObjects.end());
}

void Scene::Render() const
{
	// Build a sorted view by RenderOrder without touching ownership
	std::vector<GameObject*> sorted;
	sorted.reserve(m_pObjects.size());
	for (const auto& obj : m_pObjects)
	{
		sorted.push_back(obj.get());
	}

	std::stable_sort(sorted.begin(), sorted.end(),
		[](const GameObject* a, const GameObject* b)
		{ return a->m_RenderOrder < b->m_RenderOrder; });

	for (const auto* obj : sorted)
	{
		obj->Render();
	}
}

void dae::Scene::FixedUpdate(float fixedDelta)
{
	for (auto& object : m_pObjects)
	{
		object->FixedUpdate(fixedDelta);
	}
}