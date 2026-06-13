#pragma once
#ifndef ENEMY_TYPE_REGISTRY_H
#define ENEMY_TYPE_REGISTRY_H

#include "EnemyType.h"
#include <memory>
#include <string>
#include <unordered_map>

namespace dae
{
    class EnemyTypeRegistry final
    {
    public:
        static EnemyTypeRegistry& GetInstance()
        {
            static EnemyTypeRegistry instance;
            return instance;
        }

        void LoadFromJson(const std::string& jsonPath);

        // Returns nullptr when the id is unknown
        const EnemyType* GetType(const std::string& id) const;

        // Non-copyable singleton
        EnemyTypeRegistry(const EnemyTypeRegistry&) = delete;
        EnemyTypeRegistry& operator=(const EnemyTypeRegistry&) = delete;

    private:
        EnemyTypeRegistry() = default;

        std::unordered_map<std::string, std::unique_ptr<EnemyType>> m_Types;
    };

}

#endif
