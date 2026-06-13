#ifndef ENEMY_TYPE_H
#define ENEMY_TYPE_H

#include <string>

namespace dae
{
    class EnemyType final
    {
    public:
        EnemyType(std::string id, std::string texture,  float width,   float height)
            : m_Id { std::move(id) } , m_Texture{ std::move(texture) } , m_Width { width } , m_Height{ height }
        {}

        // Rule of 5
        EnemyType(const EnemyType&)            = delete;
        EnemyType& operator=(const EnemyType&) = delete;
        EnemyType(EnemyType&&)                 = delete;
        EnemyType& operator=(EnemyType&&)      = delete;

        const std::string& GetId()      const { return m_Id;      }
        const std::string& GetTexture() const { return m_Texture; }
        float GetWidth() const { return m_Width;   }
        float GetHeight() const { return m_Height;  }

    private:
        std::string m_Id;
        std::string m_Texture;
        float m_Width;
        float m_Height;
    };

}

#endif
