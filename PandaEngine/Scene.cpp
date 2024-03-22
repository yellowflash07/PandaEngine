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

	for (GameObject* go : m_GameObjects)
	{
		TransformComponent* transform = go->GetComponent<TransformComponent>();
		cMesh* mesh = go->GetComponent<cMesh>();
		if (mesh != nullptr)
		{
			AnimationSystem* anim = go->GetComponent<AnimationSystem>();
			if (anim != nullptr)
			{
				mesh->useBone = true;
				anim->UpdateSkeleton(mesh, *mesh->modelDrawInfo.RootNode,deltaTime);
			}
			meshManager->DrawObject(mesh, transform);
		}

		PhysicsBody* body = go->GetComponent<PhysicsBody>();
		if (body != nullptr)
		{
			phyManager->UpdatePhysicsBody(body, transform, deltaTime);
		}

		
	}
}

void Scene::Init(MeshManager* meshManager, PhysicsManager* phyManager)
{
	this->phyManager = phyManager;
	this->meshManager = meshManager;
}




