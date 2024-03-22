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
	ImGui::Begin("Scene");
	for (GameObject* go : m_GameObjects)
	{

		if (ImGui::Button(go->m_Name.c_str()))
		{
			m_pCurrentGameObject = go;
		}

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
	ImGui::End();
	ImGui::Begin("Inspector");
	if (m_pCurrentGameObject != nullptr)
	{
		DrawUI(m_pCurrentGameObject);
	}
	ImGui::End();
}

void Scene::Init(MeshManager* meshManager, PhysicsManager* phyManager)
{
	this->phyManager = phyManager;
	this->meshManager = meshManager;
}

void Scene::DrawUI(GameObject* go)
{

	ImGui::BeginChild(go->m_Name.c_str(), ImVec2(0, 20));
	ImGui::Text(go->m_Name.c_str());
	ImGui::Separator();
	ImGui::EndChild();

	for (IEditorUI* ui : go->m_UIComponents)
	{
		if (ui != nullptr)
		{
			ui->Render();
		}
	}
	
}




