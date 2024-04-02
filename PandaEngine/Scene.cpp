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

	if (ImGui::Button("Create GameObject"))
	{
		CreateGameObject("GameObject");
	}

	for (GameObject* go : m_GameObjects)
	{

		//if (ImGui::Button(go->m_Name.c_str()))

		DrawTreeNode(go);

		UpdateGameObject(go, glm::mat4(1.0f), deltaTime);

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

GameObject* Scene::GetGameObjectByName(std::string name)
{
	for (GameObject* go :  m_GameObjects)
	{
		if (go->m_Name == name)
		{
			return go;
		}
	}

	return nullptr;
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

void Scene::UpdateGameObject(GameObject* go, glm::mat4 matModel, float deltaTime)
{

	TransformComponent* transform = go->GetComponent<TransformComponent>();

	matModel = transform->GetTransform() * matModel;
	cMesh* mesh = go->GetComponent<cMesh>();
	if (mesh != nullptr)
	{
		AnimationSystem* anim = go->GetComponent<AnimationSystem>();
		if (anim != nullptr)
		{
			mesh->useBone = true;
			anim->UpdateSkeleton(mesh, *mesh->modelDrawInfo.RootNode, deltaTime);
		}

		meshManager->DrawObject(mesh, matModel);
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

	for (GameObject* child : go->m_Children)
	{
		UpdateGameObject(child, transform->GetTransform(), deltaTime);
	}
}

void Scene::DrawTreeNode(GameObject* go)
{
	if (ImGui::TreeNode(go->m_Name.c_str()))
	{
		m_pCurrentGameObject = go;
		DrawContextMenu(m_pCurrentGameObject);
		for (GameObject* child : go->m_Children)
		{
			DrawTreeNode(child);
		}
		ImGui::TreePop();
	}
}


void Scene::CreateChildObject(GameObject* go, std::string childName)
{
	GameObject* pGameObject = new GameObject(childName);
	pGameObject->entity = m_Registry.create();
	pGameObject->m_Registry = &m_Registry;
	pGameObject->AddComponent<TransformComponent>();
	go->m_Children.push_back(pGameObject);
}

void Scene::DrawContextMenu(GameObject* go)
{
	if (ImGui::BeginPopupContextItem())
	{
		if (ImGui::BeginMenu("Add Component"))
		{
			if (ImGui::MenuItem("Mesh"))
			{
				go->AddComponent<cMesh>();
			}
			if (ImGui::MenuItem("Light"))
			{
				go->AddComponent<cLight>();
			}
			if (ImGui::MenuItem("Animation System"))
			{
				go->AddComponent<AnimationSystem>();
			}
			if (ImGui::MenuItem("Child Object"))
			{
				std::string name = "Child" + go->m_Name + std::to_string(go->m_Children.size());
				CreateChildObject(go, name);
			}

			ImGui::EndMenu();
		}
		ImGui::EndPopup();
	}

	
}




