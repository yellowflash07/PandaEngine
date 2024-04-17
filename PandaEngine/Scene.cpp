#include "Scene.h"

Scene::Scene(std::string name)
{
	this->name = name;
	m_Registry = entt::registry();
	PhysXManager::getInstance()->Init(true);
	this->play = false;
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

	cLight* light = NULL;

	for (GameObject* go : m_GameObjects)
	{
		//shadow pass
		for (GameObject* golight : m_GameObjects)
		{
			if (golight->GetComponent<cLight>() != nullptr)
			{
				light = golight->GetComponent<cLight>();
				break;
			}
		}
	}
	shadowMap->BeginRender(light);
	for (GameObject* go : m_GameObjects)
	{
		//shadow pass
		cMesh* mesh = go->GetComponent<cMesh>();
		if (go->GetComponent<cMesh>() != nullptr)
		{	
			if (mesh->isSkyBox || !mesh->enableShadow)
			{
				continue;
			}
			TransformComponent* transform = go->GetComponent<TransformComponent>();
			AnimationSystem* anim = go->GetComponent<AnimationSystem>();
			if (anim != nullptr)
			{
				mesh->useBone = true;
				if (play)
				{
					anim->UpdateSkeleton(mesh, deltaTime);
				}
			}

			meshManager->DrawOnlyGeometry(go->GetComponent<cMesh>(), transform->GetTransform());
		}

	}
	shadowMap->EndRender();

	for (GameObject* go : m_GameObjects)
	{

		//if (ImGui::Button(go->m_Name.c_str()))

		DrawTreeNode(go);

		//main pass
		UpdateGameObject(go, glm::mat4(1.0f), deltaTime);

	}

	if (play)
	{
		PhysXManager::getInstance()->Update(deltaTime);
		
	}
	ImGui::Checkbox("Debug Physics", &debugPhysics);
	if (debugPhysics)
	{
		PhysXManager::getInstance()->DebugUpdate(deltaTime);
		PhysXManager::getInstance()->DrawDebug();
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
	cShaderManager* shaderManager, GLFWwindow* window)
{
	this->phyManager = phyManager;
	this->meshManager = meshManager;
	this->lightManager = lightManager;
	this->shaderManager = shaderManager;

	this->shadowMap = new ShadowMap();
	this->shadowMap->Initialize(2048 , 2048 );
	this->shadowMap->shaderProgramID = shaderManager->getIDFromFriendlyName("shader01");
	this->shadowMap->window = window;
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

GameObject* Scene::NewGameObject(std::string name)
{
	GameObject* go = new GameObject(name);
	go->entity = m_Registry.create();
	go->m_Registry = &m_Registry;
	return go;
}

void Scene::AddGameObject(GameObject* go)
{
	m_GameObjects.push_back(go);
}

void Scene::Play()
{
	this->play = true;
}

void Scene::Stop()
{
	this->play = false;
}

void Scene::DrawUI(GameObject* go)
{

	ImGui::BeginChild(go->m_Name.c_str(), ImVec2(0, 40));
	ImGui::Text(go->m_Name.c_str());
	std::string friendName = go->m_Name;
	if (ImGui::InputText("Name", &friendName[0], 100, ImGuiInputTextFlags_::ImGuiInputTextFlags_EnterReturnsTrue))
	{
		go->m_Name = friendName.c_str();
	}
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
		meshManager->DrawObject(mesh, matModel);
		AnimationSystem* anim = go->GetComponent<AnimationSystem>();
		if (anim != nullptr)
		{
			mesh->useBone = true;
			anim->SetMesh(mesh);
			if (play)
			{
				anim->UpdateSkeleton(mesh, deltaTime);
			}
		}
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

	if (play)
	{
		PhysXBody* physXBody = go->GetComponent<PhysXBody>();
		if (physXBody != nullptr)
		{
			physXBody->Update(deltaTime);
		}

		CharacterController* controller = go->GetComponent<CharacterController>();
		if (controller != nullptr)
		{
			controller->Update(deltaTime);
		}
	}
	

	glm::mat4 matRemoveScaling = glm::scale(glm::mat4(1.0f),
		glm::vec3(
			1.0f / transform->drawScale.x,
			1.0f / transform->drawScale.y,
			1.0f / transform->drawScale.z));

	matModel = matModel * matRemoveScaling;

	for (GameObject* child : go->m_Children)
	{
		UpdateGameObject(child, matModel, deltaTime);
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


GameObject* Scene::CreateChildObject(GameObject* go, std::string childName)
{
	GameObject* pGameObject = new GameObject(childName);
	pGameObject->entity = m_Registry.create();
	pGameObject->m_Registry = &m_Registry;
	pGameObject->AddComponent<TransformComponent>();
	go->m_Children.push_back(pGameObject);
	return pGameObject;
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

			if (ImGui::MenuItem("Child Object"))
			{
				std::string name = "Child" + go->m_Name + std::to_string(go->m_Children.size());
				CreateChildObject(go, name);
			}


			cMesh* mesh = go->GetComponent<cMesh>();
			if (mesh != nullptr)
			{
				if (ImGui::MenuItem("Animation System"))
				{
					mesh->useBone = true;
					go->AddComponent<AnimationSystem>();
				}
				if (ImGui::MenuItem("Physics Body"))
				{
					PhysXBody* pxBody = &go->AddComponent<PhysXBody>(go->GetComponent<TransformComponent>());
					pxBody->mesh = mesh;
				}
				if (ImGui::MenuItem("Character Controller"))
				{
					go->AddComponent<CharacterController>(go->GetComponent<TransformComponent>());
				}
			}

			if (ImGui::MenuItem("Remove Object"))
			{
				//go->Destroy();
				m_GameObjects.erase(std::remove(m_GameObjects.begin(), m_GameObjects.end(), go), m_GameObjects.end());
			}


			ImGui::EndMenu();
		}
		ImGui::EndPopup();
	}

	
}




