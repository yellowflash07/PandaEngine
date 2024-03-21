#pragma once

#include <entt/entt.hpp>
#include <vector>
#include "GameObject.h"
#include "TransformComponent.h"
class Scene
{
public:
	Scene(std::string name);
	~Scene();


	entt::registry& GetRegistry() { return m_Registry; }

	GameObject* CreateGameObject(std::string name);
	//void DestroyGameObject(GameObject* pGameObject);
private:
	entt::registry m_Registry;
	entt::entity m_currentEntity;
	std::string name;
	std::vector<GameObject*> m_GameObjects;
};

