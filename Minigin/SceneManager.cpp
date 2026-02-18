#include "SceneManager.h"
#include "Scene.h"

void dae::SceneManager::Update(float deltaTime)
{
	for(auto& scene : m_pScenes)
	{
		scene->Update(deltaTime);
	}
}

void dae::SceneManager::Render()
{
	for (const auto& scene : m_pScenes)
	{
		scene->Render();
	}
}

dae::Scene& dae::SceneManager::CreateScene()
{
	m_pScenes.emplace_back(new Scene());
	return *m_pScenes.back();
}

void dae::SceneManager::FixedUpdate(float fixedDelta)
{
	for (auto& scene : m_pScenes)
	{
		scene->FixedUpdate(fixedDelta);
	}
}