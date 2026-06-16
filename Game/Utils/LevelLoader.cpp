#include "LevelLoader.h"
#include "JSONParser.h"
#include "EnemyTypeRegistry.h"
#include "Components/EnemyComponent.h"
#include "Components/EnemyAIComponent.h"

#include "Scene.h"
#include "GameObject.h"
#include "TextureComponent.h"

#include "Components/LevelCollisionComponent.h"
#include "Components/LevelDebugDrawComponent.h"
#include "Components/PhysicsComponent.h"
#include "Components/BoxColliderComponent.h"
#include "Components/BoxColliderDebugDrawComponent.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <memory>

namespace dae
{
	float LevelLoader::EvalSpawnX(const std::string& expr, float windowWidth)
	{
		std::string e = expr;
		const std::string token = "windowWidth";
		const std::string value = std::to_string(static_cast<int>(windowWidth));
		size_t pos = 0;
		while ((pos = e.find(token, pos)) != std::string::npos)
		{
			e.replace(pos, token.size(), value);
			pos += value.size();
		}

		// Tokenise into (operator, operand) pairs
		std::istringstream ss(e);
		std::vector<std::pair<char, float>> tokens;
		char  curOp = '+';
		float val = 0.f;
		while (ss >> val)
		{
			tokens.push_back({ curOp, val });
			char c{};
			if (ss >> c) curOp = c;
		}

		// Pass 1: resolve * and /
		for (size_t i = 1; i < tokens.size(); )
		{
			if (tokens[i].first == '*')
			{
				tokens[i - 1].second *= tokens[i].second;
				tokens.erase(tokens.begin() + static_cast<ptrdiff_t>(i));
			}
			else if (tokens[i].first == '/')
			{
				if (tokens[i].second != 0.f)
					tokens[i - 1].second /= tokens[i].second;
				tokens.erase(tokens.begin() + static_cast<ptrdiff_t>(i));
			}
			else ++i;
		}

		// Pass 2: resolve + and -
		float result = 0.f;
		for (const auto& [o, v] : tokens)
		{
			if (o == '+') result += v;
			else if (o == '-') result -= v;
		}

		return result;
	}

	LevelLoader::LevelLoader(const std::string& jsonPath)
		: m_JsonPath{ jsonPath }
	{
		// Load type registry once at startup
		EnemyTypeRegistry::GetInstance().LoadFromJson(jsonPath);

		// Count levels by parsing the file
		EnemiesFileData data;
		if (JSONParser::ParseEnemiesFile(jsonPath, data))
		{
			m_LevelCount = static_cast<int>(data.levels.size());
		}
	}

	int LevelLoader::GetLevelCount() const { return m_LevelCount; }

	LevelLoader::LevelResult LevelLoader::LoadLevel(Scene& scene,int levelIndex,float windowWidth, float windowHeight, const std::vector<GameObject*>& players)
	{
		LevelResult result{};

		EnemiesFileData data;
		if (!JSONParser::ParseEnemiesFile(m_JsonPath, data))
		{
			std::cerr << "[LevelLoader] Failed to parse " << m_JsonPath << '\n';
			return result;
		}

		if (levelIndex < 0 || levelIndex >= static_cast<int>(data.levels.size()))
		{
			std::cerr << "[LevelLoader] Level index " << levelIndex << " out of range\n";
			return result;
		}

		const LevelData& levelData = data.levels[levelIndex];

		// Background 
		{
			auto bg = std::make_unique<GameObject>();
			bg->SetLocalPosition(0.f, windowHeight);
			bg->AddComponent<TextureComponent>(windowWidth, windowHeight)->SetTexture(levelData.backgroundTexture);
			result.levelObjects.push_back(bg.get());
			scene.Add(std::move(bg));
		}

		// Level collision 
		auto levelGO = std::make_unique<GameObject>();
		levelGO->SetLocalPosition(0.f, windowHeight);
		levelGO->AddComponent<TextureComponent>(windowWidth, windowHeight)->SetTexture(levelData.backgroundTexture);

		auto* pLevelCol = levelGO->AddComponent<LevelCollisionComponent>();
		pLevelCol->LoadFromSVG(levelData.collisionSVG, windowWidth, windowHeight);
		//levelGO->AddComponent<LevelDebugDrawComponent>(pLevelCol);
		result.levelObjects.push_back(levelGO.get());
		scene.Add(std::move(levelGO));

		result.pCollision = pLevelCol;

		// Wire all players to the new collision geometry
		for (auto* pPlayer : players)
		{
			if (auto* phys = pPlayer->GetComponent<PhysicsComponent>())
			{
				phys->SetLevelCollision(pLevelCol);
			}
		}


		// Enemies
		for (const auto& spawnData : levelData.enemies)
		{
			const EnemyType* pType = EnemyTypeRegistry::GetInstance().GetType(spawnData.typeId);
			if (!pType)
			{
				std::cerr << "[LevelLoader] Unknown enemy type '"
					<< spawnData.typeId << "', skipping.\n";
				continue;
			}

			const float SPAWN_X = EvalSpawnX(spawnData.spawnExprX, windowWidth);
			const float SPAWN_Y = spawnData.spawnY;

			auto enemyGO = std::make_unique<GameObject>();
			enemyGO->SetLocalPosition(SPAWN_X, SPAWN_Y);
			enemyGO->AddComponent<TextureComponent>(pType->GetWidth(), pType->GetHeight()) ->SetTexture(pType->GetTexture());

			auto* pPhys = enemyGO->AddComponent<PhysicsComponent>(windowHeight);
			pPhys->SetLevelCollision(pLevelCol);

			enemyGO->AddComponent<BoxColliderComponent>(pType->GetWidth(), pType->GetHeight());
			//enemyGO->AddComponent<BoxColliderDebugDrawComponent>(255, 128, 0);

			enemyGO->AddComponent<EnemyComponent>(pType);
			enemyGO->AddComponent<EnemyAIComponent>(scene, pType->GetId());


			result.enemies.push_back(enemyGO.get());
			result.levelObjects.push_back(enemyGO.get());
			scene.Add(std::move(enemyGO));
		}

		std::cout << "[LevelLoader] Level " << levelIndex << " loaded: " << result.enemies.size() << " enemy/enemies.\n";

		return result;
	}

}