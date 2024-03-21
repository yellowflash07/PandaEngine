#include "Scene.h"

Scene::Scene(std::string name)
{
	this->name = name;
	m_Registry = entt::registry();
}

Scene::~Scene()
{
}

GameObject* Scene::CreateGameObject(std::string name)
{
	GameObject* pGameObject = new GameObject(name);
	pGameObject->entity = m_Registry.create();
	pGameObject->m_Registry = &m_Registry;
	pGameObject->AddComponent<TransformComponent>();
	m_GameObjects.push_back(pGameObject);
	return pGameObject;
}

void Scene::Update(float deltaTime)
{
	for (auto go : m_GameObjects)
	{
		TransformComponent* transform = &go->GetComponent<TransformComponent>();
		cMesh* mesh = &go->GetComponent<cMesh>();
		if (mesh != nullptr)
		{
			meshManager->DrawObject(mesh, transform);
		}
	}
}

void Scene::Init(MeshManager* meshManager)
{
	this->meshManager = meshManager;
}




