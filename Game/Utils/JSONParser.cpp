#include "JSONParser.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <stdexcept>

bool JSONParser::ParseEnemiesFile(const std::string& filePath, EnemiesFileData& out)
{
	std::ifstream file(filePath);
	if (!file)
	{
		std::cerr << "JSONParser::ParseEnemiesFile, failed to open file: " << filePath << '\n';
		return false;
	}

	std::string json((std::istreambuf_iterator<char>(file)),
	                  std::istreambuf_iterator<char>());
	file.close();

	Normalize(json);

	if (!ParseEnemyTypes(json, out.enemyTypes))
	{
		std::cerr << "JSONParser::ParseEnemiesFile, failed to parse enemyTypes in: " << filePath << '\n';
		return false;
	}

	if (!ParseLevels(json, out.levels))
	{
		std::cerr << "JSONParser::ParseEnemiesFile, failed to parse levels in: " << filePath << '\n';
		return false;
	}

	return true;
}

// ────────────────────────────────────────────────────────────────────────────
//  Top-level section parsers
// ────────────────────────────────────────────────────────────────────────────
bool JSONParser::ParseEnemyTypes(const std::string& json, std::vector<EnemyTypeData>& out)
{
	std::string arrayContent;
	if (!GetArrayContent(json, "enemyTypes", arrayContent))
	{
		std::cerr << "JSONParser::ParseEnemyTypes, \"enemyTypes\" array not found\n";
		return false;
	}

	std::vector<std::string> objects;
	if (!SplitObjects(arrayContent, objects))
	{
		std::cerr << "JSONParser::ParseEnemyTypes, failed to split objects\n";
		return false;
	}

	for (const auto& obj : objects)
	{
		EnemyTypeData data;
		if (!ParseEnemyTypeObject(obj, data))
		{
			std::cerr << "JSONParser::ParseEnemyTypes, failed to parse object: " << obj << '\n';
			return false;
		}
		out.push_back(data);
	}

	return true;
}

bool JSONParser::ParseLevels(const std::string& json, std::vector<LevelData>& out)
{
	std::string arrayContent;
	if (!GetArrayContent(json, "levels", arrayContent))
	{
		std::cerr << "JSONParser::ParseLevels, \"levels\" array not found\n";
		return false;
	}

	std::vector<std::string> objects;
	if (!SplitObjects(arrayContent, objects))
	{
		std::cerr << "JSONParser::ParseLevels, failed to split level objects\n";
		return false;
	}

	for (const auto& obj : objects)
	{
		LevelData data;
		if (!ParseLevelObject(obj, data))
		{
			std::cerr << "JSONParser::ParseLevels, failed to parse level object: " << obj << '\n';
			return false;
		}
		out.push_back(data);
	}

	return true;
}

// ────────────────────────────────────────────────────────────────────────────
//  Object parsers
// ────────────────────────────────────────────────────────────────────────────
bool JSONParser::ParseEnemyTypeObject(const std::string& obj, EnemyTypeData& out)
{
	if (!GetStringValue(obj, "id",      out.id))      { std::cerr << "JSONParser::ParseEnemyTypeObject, missing \"id\"\n";      return false; }
	if (!GetStringValue(obj, "texture", out.texture)) { std::cerr << "JSONParser::ParseEnemyTypeObject, missing \"texture\"\n"; return false; }
	if (!GetFloatValue (obj, "width",   out.width))   { std::cerr << "JSONParser::ParseEnemyTypeObject, missing \"width\"\n";   return false; }
	if (!GetFloatValue (obj, "height",  out.height))  { std::cerr << "JSONParser::ParseEnemyTypeObject, missing \"height\"\n";  return false; }
	return true;
}

bool JSONParser::ParseLevelObject(const std::string& obj, LevelData& out)
{
	if (!GetIntValue   (obj, "index",             out.index))             { std::cerr << "JSONParser::ParseLevelObject, missing \"index\"\n";             return false; }
	if (!GetStringValue(obj, "backgroundTexture", out.backgroundTexture)) { std::cerr << "JSONParser::ParseLevelObject, missing \"backgroundTexture\"\n"; return false; }
	if (!GetStringValue(obj, "collisionSVG",      out.collisionSVG))      { std::cerr << "JSONParser::ParseLevelObject, missing \"collisionSVG\"\n";      return false; }

	// Parse the nested enemies array inside this level object
	std::string enemiesArray;
	if (!GetArrayContent(obj, "enemies", enemiesArray))
	{
		std::cerr << "JSONParser::ParseLevelObject, missing \"enemies\" array\n";
		return false;
	}

	std::vector<std::string> enemyObjects;
	if (!SplitObjects(enemiesArray, enemyObjects))
	{
		std::cerr << "JSONParser::ParseLevelObject, failed to split enemy objects\n";
		return false;
	}

	for (const auto& eObj : enemyObjects)
	{
		EnemySpawnData spawn;
		if (!ParseEnemySpawn(eObj, spawn))
		{
			std::cerr << "JSONParser::ParseLevelObject, failed to parse enemy spawn: " << eObj << '\n';
			return false;
		}
		out.enemies.push_back(spawn);
	}

	return true;
}

bool JSONParser::ParseEnemySpawn(const std::string& obj, EnemySpawnData& out)
{
	if (!GetStringValue(obj, "type",       out.typeId))     { std::cerr << "JSONParser::ParseEnemySpawn, missing \"type\"\n";       return false; }
	if (!GetStringValue(obj, "spawnExprX", out.spawnExprX)) { std::cerr << "JSONParser::ParseEnemySpawn, missing \"spawnExprX\"\n"; return false; }
	if (!GetFloatValue (obj, "spawnY",     out.spawnY))     { std::cerr << "JSONParser::ParseEnemySpawn, missing \"spawnY\"\n";     return false; }
	return true;
}

// ────────────────────────────────────────────────────────────────────────────
//  Primitive extractors
// ────────────────────────────────────────────────────────────────────────────

// Finds:  "key" : "some value"
//                  ^^^^^^^^^^  returned in out
bool JSONParser::GetStringValue(const std::string& json, const std::string& key, std::string& out)
{
	// Search for  "key"
	const std::string searchKey = "\"" + key + "\"";
	const size_t keyPos = json.find(searchKey);
	if (keyPos == std::string::npos) return false;

	// Find the colon after the key
	const size_t colonPos = json.find(':', keyPos + searchKey.size());
	if (colonPos == std::string::npos) return false;

	// Find the opening quote of the value
	const size_t openQuote = json.find('"', colonPos + 1);
	if (openQuote == std::string::npos) return false;

	// Find the closing quote (skip escaped quotes)
	size_t closeQuote = openQuote + 1;
	while (closeQuote < json.size())
	{
		if (json[closeQuote] == '"' && json[closeQuote - 1] != '\\')
			break;
		++closeQuote;
	}
	if (closeQuote >= json.size()) return false;

	out = json.substr(openQuote + 1, closeQuote - openQuote - 1);
	return true;
}

// Finds:  "key" : 123.4
//                 ^^^^^  returned in out
bool JSONParser::GetFloatValue(const std::string& json, const std::string& key, float& out)
{
	const std::string searchKey = "\"" + key + "\"";
	const size_t keyPos = json.find(searchKey);
	if (keyPos == std::string::npos) return false;

	const size_t colonPos = json.find(':', keyPos + searchKey.size());
	if (colonPos == std::string::npos) return false;

	// Skip whitespace after colon to reach the number
	size_t numStart = colonPos + 1;
	while (numStart < json.size() && std::isspace(static_cast<unsigned char>(json[numStart])))
		++numStart;

	if (numStart >= json.size()) return false;

	// stof reads until it hits a non-numeric character, which is what we want
	try
	{
		size_t charsRead{};
		out = std::stof(json.substr(numStart), &charsRead);
		if (charsRead == 0) return false;
	}
	catch (const std::invalid_argument&) { return false; }
	catch (const std::out_of_range&)     { return false; }

	return true;
}

// Finds:  "key" : 2
//                 ^  returned in out
bool JSONParser::GetIntValue(const std::string& json, const std::string& key, int& out)
{
	float f{};
	if (!GetFloatValue(json, key, f)) return false;
	out = static_cast<int>(f);
	return true;
}

// ────────────────────────────────────────────────────────────────────────────
//  Array / block helpers
// ────────────────────────────────────────────────────────────────────────────

// Finds:  "key" : [ ... ]
//                   ^^^  returned in out (without the brackets)
bool JSONParser::GetArrayContent(const std::string& json, const std::string& key, std::string& out)
{
	const std::string searchKey = "\"" + key + "\"";
	const size_t keyPos = json.find(searchKey);
	if (keyPos == std::string::npos) return false;

	const size_t colonPos = json.find(':', keyPos + searchKey.size());
	if (colonPos == std::string::npos) return false;

	const size_t openBracket = json.find('[', colonPos + 1);
	if (openBracket == std::string::npos) return false;

	// Walk forward counting bracket depth to find the matching close bracket
	int depth = 1;
	size_t i = openBracket + 1;
	while (i < json.size() && depth > 0)
	{
		if      (json[i] == '[') ++depth;
		else if (json[i] == ']') --depth;
		++i;
	}

	if (depth != 0)
	{
		std::cerr << "JSONParser::GetArrayContent, unmatched '[' for key \"" << key << "\"\n";
		return false;
	}

	// i is now one past the closing ']', so the content is [openBracket+1, i-2]
	out = json.substr(openBracket + 1, i - openBracket - 2);
	return true;
}

// Splits an array body (text between [ and ]) into individual { ... } strings.
// Handles nested objects and arrays correctly via depth counting.
bool JSONParser::SplitObjects(const std::string& arrayContent, std::vector<std::string>& objects)
{
	size_t i = 0;
	const size_t len = arrayContent.size();

	while (i < len)
	{
		// Skip until we hit '{'
		while (i < len && arrayContent[i] != '{') ++i;
		if (i >= len) break;

		// Found the start of an object — walk to its matching '}'
		const size_t objStart = i;
		int depth = 1;
		++i;

		while (i < len && depth > 0)
		{
			if      (arrayContent[i] == '{') ++depth;
			else if (arrayContent[i] == '}') --depth;
			++i;
		}

		if (depth != 0)
		{
			std::cerr << "JSONParser::SplitObjects, unmatched '{' in array content\n";
			return false;
		}

		// i is now one past the closing '}', so the object is [objStart, i-1)
		objects.push_back(arrayContent.substr(objStart, i - objStart));
	}

	return true;
}

// ────────────────────────────────────────────────────────────────────────────
//  Normalize — strips comments, collapses whitespace
// ────────────────────────────────────────────────────────────────────────────
void JSONParser::Normalize(std::string& json)
{
	std::string result;
	result.reserve(json.size());

	bool inString = false;
	size_t i = 0;
	const size_t len = json.size();

	while (i < len)
	{
		const char c = json[i];

		// Track whether we're inside a quoted string so we don't
		// accidentally strip content that looks like a comment
		if (c == '"' && (i == 0 || json[i - 1] != '\\'))
			inString = !inString;

		if (!inString)
		{
			// Strip  // line comments
			if (c == '/' && i + 1 < len && json[i + 1] == '/')
			{
				while (i < len && json[i] != '\n') ++i;
				continue;
			}
			// Strip  /* block comments */
			if (c == '/' && i + 1 < len && json[i + 1] == '*')
			{
				i += 2;
				while (i + 1 < len && !(json[i] == '*' && json[i + 1] == '/')) ++i;
				i += 2;
				continue;
			}
		}

		result += c;
		++i;
	}

	json = std::move(result);
}
