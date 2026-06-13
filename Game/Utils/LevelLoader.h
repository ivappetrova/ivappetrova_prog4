#pragma once
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

	// -----------------------------------------------------------------------
	// LevelLoader
	//
	// Reads enemies.json once, then on request builds all GameObjects
	// (background, level collision, enemies) for a given level index.
	//
	// Spawn X expressions like "windowWidth/2 - 70" are evaluated at load
	// time with the actual window dimensions passed in.
	//
	// Usage:
	//   LevelLoader loader("Data/enemies.json");
	//   loader.LoadLevel(scene, 0, windowW, windowH,
	//                    players,          // to wire physics
	//                    onLevelComplete); // called when all enemies die
	// -----------------------------------------------------------------------
	class LevelLoader final
	{
	public:
		explicit LevelLoader(const std::string& jsonPath);

		int GetLevelCount() const;

		// Returned by LoadLevel so callers can wire physics and track enemies.
		struct LevelResult
		{
			LevelCollisionComponent* pCollision{ nullptr };
			std::vector<GameObject*>  enemies;      // raw ptrs — scene owns lifetime
			std::vector<GameObject*>  levelObjects; // all level-owned GOs (bg, collision, enemies)
		};

		// Spawns everything for levelIndex into scene.
		// players : raw ptrs to all player GameObjects (physics wired here)
		// Returns a LevelResult with the collision component and enemy list.
		LevelResult LoadLevel(
			Scene& scene,
			int                             levelIndex,
			float                           windowWidth,
			float                           windowHeight,
			const std::vector<GameObject*>& players);

	private:
		// Evaluates simple spawn-X expressions like "windowWidth/2 - 70"
		static float EvalSpawnX(const std::string& expr, float windowWidth);

		std::string m_JsonPath;
		int         m_LevelCount{ 0 };
	};

} // namespace dae

#endif // LEVEL_LOADER_H