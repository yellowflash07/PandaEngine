#pragma once

#include <entt/entt.hpp>
#include <vector>
#include "GameObject.h"
#include "TransformComponent.h"
#include "MeshManager.h"
class Scene
{
public:
	Scene(std::string name);
	~Scene();

	entt::registry& GetRegistry() { return m_Registry; }

	GameObject* CreateGameObject(std::string name);

	void Update(float deltaTime);
	MeshManager* meshManager;

	void Init(MeshManager* meshManager);

private:
	entt::registry m_Registry;
	entt::entity m_currentEntity;
	std::string name;
	std::vector<GameObject*> m_GameObjects;
};

