#ifndef JSON_PARSER_H
#define JSON_PARSER_H

#include <string>
#include <vector>

///////////////////////////////////////////////////////// Reading

struct EnemySpawnData
{
	std::string typeId;     // zenchan or maita
	std::string spawnExprX; 
	float spawnY{};
};

// Describes one enemy type (shared data, loaded once)
struct EnemyTypeData
{
	std::string id;
	std::string texture;
	float width{};
	float height{};
};

// Describes one level entry
struct LevelData
{
	int index{};
	std::string backgroundTexture;
	std::string collisionSVG;
	std::vector<EnemySpawnData> enemies;
};

// Top-level result of parsing enemies.json
struct EnemiesFileData
{
	std::vector<EnemyTypeData> enemyTypes;
	std::vector<LevelData> levels;
};


////////////////////////////////////////////////////// Writing

struct ScoreEntryData
{
	std::string name;
	int score{};
	std::string mode;
};

struct ScoresFileData
{
	std::vector<ScoreEntryData> entries;
};

class JSONParser final
{
public:
	static bool ParseEnemiesFile(const std::string& filePath, EnemiesFileData& out);

	static bool ParseScoresFile(const std::string& filePath, ScoresFileData& out);
	static bool WriteScoresFile(const std::string& filePath, const ScoresFileData& data);


private:
	static bool ParseEnemyTypes(const std::string& json, std::vector<EnemyTypeData>& out);
	static bool ParseLevels(const std::string& json, std::vector<LevelData>& out);
	static bool ParseEnemyTypeObject(const std::string& obj, EnemyTypeData& out);
	static bool ParseLevelObject(const std::string& obj, LevelData& out);
	static bool ParseEnemySpawn(const std::string& obj, EnemySpawnData& out);

	static bool GetStringValue(const std::string& json, const std::string& key, std::string& out);
	static bool GetFloatValue(const std::string& json, const std::string& key, float& out);
	static bool GetIntValue(const std::string& json, const std::string& key, int& out);
	static bool GetArrayContent(const std::string& json, const std::string& key, std::string& out);
	static bool SplitObjects(const std::string& arrayContent, std::vector<std::string>& objects);
	static void Normalize(std::string& json);


};
#endif