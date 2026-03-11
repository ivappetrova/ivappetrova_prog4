#pragma	once
#include "Command.h"

namespace dae
{
	class GameObject;
	class GameObjectCommand : public Command
	{
	public:
		explicit GameObjectCommand(GameObject* gameObject) : m_pGameObject(gameObject) {}
		virtual ~GameObjectCommand() = default;
	
		GameObjectCommand() = default;
		GameObjectCommand(const GameObjectCommand&) = delete;
		GameObjectCommand(GameObjectCommand&&) = delete;
		GameObjectCommand& operator=(const GameObjectCommand&) = delete;
		GameObjectCommand& operator=(GameObjectCommand&&) = delete;
	
	protected:
		GameObject* GetGameObject() const { return m_pGameObject; }
	
	private:
		GameObject* m_pGameObject;
	};
}