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

		UpdateGameObject(go, deltaTime);

		for (GameObject* child : go->m_Children)
		{
			UpdateGameObject(child, deltaTime);
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

void Scene::Init(MeshManager* meshManager, PhysicsManager* phyManager, cLightManager* lightManager,
	cShaderManager* shaderManager)
{
	this->phyManager = phyManager;
	this->meshManager = meshManager;
	this->lightManager = lightManager;
	this->shaderManager = shaderManager;
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

void Scene::UpdateGameObject(GameObject* go, float deltaTime)
{

	TransformComponent* transform = go->GetComponent<TransformComponent>();
	cMesh* mesh = go->GetComponent<cMesh>();
	if (mesh != nullptr)
	{
		AnimationSystem* anim = go->GetComponent<AnimationSystem>();
		if (anim != nullptr)
		{
			mesh->useBone = true;
			anim->UpdateSkeleton(mesh, *mesh->modelDrawInfo.RootNode, deltaTime);
		}
		meshManager->DrawObject(mesh, transform);
	}

	PhysicsBody* body = go->GetComponent<PhysicsBody>();
	if (body != nullptr)
	{
		phyManager->UpdatePhysicsBody(body, transform, deltaTime);
	}

	cLight* light = go->GetComponent<cLight>();
	if (light != nullptr)
	{
		if (!light->uniformLocationIsSet)
		{
			light->uniformLocationIsSet = true;
			GLint shaderID = shaderManager->getIDFromFriendlyName("shader01");
			light->SetUniformLocations(shaderID, lightIndex);
			lightIndex++;
		}

		light->UpdateLight(transform);
	}
}




