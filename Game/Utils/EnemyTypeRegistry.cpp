#include "EnemyTypeRegistry.h"
#include "JSONParser.h"
#include <iostream>

namespace dae
{
    void EnemyTypeRegistry::LoadFromJson(const std::string& jsonPath)
    {
        EnemiesFileData data;
        if (!JSONParser::ParseEnemiesFile(jsonPath, data))
        {
            std::cerr << "[EnemyTypeRegistry] Failed to parse: " << jsonPath << std::endl;
            return;
        }

        m_Types.clear();

		for (const auto& t : data.enemyTypes)
		{
            m_Types[t.id] = std::make_unique<EnemyType>(t.id, t.texture, t.width, t.height);
		}

        std::cout << "[EnemyTypeRegistry] Loaded " << m_Types.size() << " enemy type(s).\n";
    }

    const EnemyType* EnemyTypeRegistry::GetType(const std::string& id) const
    {
        const auto it {m_Types.find(id)};
        if (it == m_Types.end())
        {
            std::cerr << "[EnemyTypeRegistry] Unknown enemy type: " << id << '\n';
            return nullptr;
        }
        return it->second.get();
    }

}
