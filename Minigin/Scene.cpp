#include <algorithm>
#include "Scene.h"

using namespace dae;

void Scene::Add(std::unique_ptr<GameObject> object)
{
	assert(object != nullptr && "Cannot add a null GameObject to the scene.");
	m_pObjects.emplace_back(std::move(object));
}

void Scene::Remove(const GameObject& object)
{
	m_pObjects.erase(
		std::remove_if(m_pObjects.begin(), m_pObjects.end(), [&object](const auto& ptr) { return ptr.get() == &object; }), m_pObjects.end() );
}

void Scene::RemoveAll()
{
	m_pObjects.clear();
}

void Scene::Update(float deltaTime)
{
	for(auto& objectPtr : m_pObjects)
	{
		objectPtr->Update(deltaTime);
	}
}

void Scene::Render() const
{
	for (const auto& OBJECT_PTR : m_pObjects)
	{
		OBJECT_PTR->Render();
	}
}

void dae::Scene::FixedUpdate(float fixedDelta)
{
	for (auto& object : m_pObjects)
	{
		object->FixedUpdate(fixedDelta);
	}
}


