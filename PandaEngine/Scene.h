#pragma once

#include <entt/entt.hpp>
#include <vector>
#include "GameObject.h"
#include "TransformComponent.h"
#include "MeshManager.h"
#include "PhysicsManager.h"
#include "AnimationSystem.h"
#include "cLightManager.h"
#include "../Basic Shader Manager/cShaderManager.h"
#include <imgui.h>

class Scene
{
public:
	Scene(std::string name);
	~Scene();

	entt::registry& GetRegistry() { return m_Registry; }

	GameObject* CreateGameObject(std::string name);

	void Update(float deltaTime);

	std::vector<GameObject*> GetGameObjects() { return m_GameObjects; }

	std::string name;

	void Init(MeshManager* meshManager, PhysicsManager* phyManager, cLightManager* lightManager
	 , cShaderManager* shaderManager);

	GameObject* GetGameObjectByName(std::string name);

private:
	entt::registry m_Registry;
	entt::entity m_currentEntity;
	std::vector<GameObject*> m_GameObjects;
	MeshManager* meshManager;
	PhysicsManager* phyManager;
	cLightManager* lightManager;
	cShaderManager* shaderManager;
	GameObject* m_pCurrentGameObject;
	int lightIndex = 0;
	void DrawUI(GameObject* go);

	void UpdateGameObject(GameObject* go, float deltaTime);

	void CreateChildObject(GameObject* go, std::string childName);

	void DrawContextMenu(GameObject* go);
};

