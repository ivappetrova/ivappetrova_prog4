#ifndef LEVEL_LOADER_H
#define LEVEL_LOADER_H

#include <string>
#include <vector>
#include <functional>

namespace dae
{
	class Scene;
	class GameObject;
	class LevelCollisionComponent;

	class LevelLoader final
	{
	public:
		explicit LevelLoader(const std::string& jsonPath);

		int GetLevelCount() const;

		struct LevelResult
		{
			LevelCollisionComponent* pCollision{ nullptr };
			std::vector<GameObject*> enemies;      
			std::vector<GameObject*> levelObjects;
		};

		// Spawns everything for levelIndex into scene
		LevelResult LoadLevel(Scene& scene,int levelIndex, float windowWidth, float windowHeight, const std::vector<GameObject*>& players);

	private:
		static float EvalSpawnX(const std::string& expr, float windowWidth);

		std::string m_JsonPath;
		int m_LevelCount{ 0 };
	};

}

#endif