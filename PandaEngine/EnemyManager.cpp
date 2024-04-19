#include "EnemyManager.h"

EnemyManager::EnemyManager(Scene* scene, GameObject* player)
	: m_Scene(scene), m_Player(player)
{
	m_EnemySpawnPoints.push_back(glm::vec3(glm::vec3(-1514, -86.11502075195313, 11.56)));
	m_EnemySpawnPoints.push_back(glm::vec3(glm::vec3(-600, -86, 2066)));
	m_EnemySpawnPoints.push_back(glm::vec3(glm::vec3(1846, -86, 1766)));
	m_EnemySpawnPoints.push_back(glm::vec3(glm::vec3(2081, -86, 417)));
	m_EnemySpawnPoints.push_back(glm::vec3(glm::vec3(2081, -86, 417)));

}

EnemyManager::~EnemyManager()
{
}

void EnemyManager::Update(float deltaTime)
{
	if (m_Enemies.size() < m_EnemyCount)
	{
		CreateEnemy();
	}

	for (GameObject* enemy : m_Enemies)
	{
		if (m_EnemyStates[enemy] == EnemyState::ALIVE)
		{
			TransformComponent* transform = enemy->GetComponent<TransformComponent>();
			PhysXBody* physXBody = enemy->GetComponent<PhysXBody>();
			TransformComponent* playerTransform = m_Player->GetComponent<TransformComponent>();
			glm::vec3 direction = playerTransform->drawPosition - transform->drawPosition;
			direction = glm::normalize(direction);
			
			glm::quat rotation = glm::quatLookAt(-direction, glm::vec3(0.0f, 1.0f, 0.0f));
			transform->eulerRotation = glm::eulerAngles(rotation);

			glm::vec3 enemyPosition = direction * speed;
			//move the enemy towards the target
			transform->drawPosition += enemyPosition;

			//physXBody->position += direction * speed;
			physXBody->body->setGlobalPose(PxTransform(PxVec3(transform->drawPosition.x, transform->drawPosition.y, transform->drawPosition.z)));
		}
		else if (m_EnemyStates[enemy] == EnemyState::DEAD)
		{
			//enemy->GetComponent<PhysXBody>()->Update(deltaTime);
			//enemy->GetComponent<AnimationSystem>()->Update(deltaTime);
		}
	}
}

void EnemyManager::AddEnemy()
{

}

void EnemyManager::RemoveEnemy(GameObject* enemy)
{
	PhysXBody* physXBody = enemy->GetComponent<PhysXBody>();
	physXBody->isTrigger = true;
	physXBody->SetTrigger();
	m_Enemies.erase(std::remove(m_Enemies.begin(), m_Enemies.end(), enemy), m_Enemies.end());
	m_Scene->DestroyGameObject(enemy);
}


void EnemyManager::CreateEnemy()
{
	GameObject* enemy = m_Scene->CreateGameObject(this->name + std::to_string(currentEnemyCount));
	enemy->tag = "Zombie";
	TransformComponent* transform = enemy->GetComponent<TransformComponent>();	
	transform->drawPosition = m_EnemySpawnPoints[rand() % m_EnemySpawnPoints.size()];
	transform->eulerRotation = this->rotation;
	transform->drawScale = this->scale;

	cMesh* mesh = &enemy->AddComponent<cMesh>(this->meshFileName, this->name);
	mesh->normalMap = this->normalMapFileName;
	mesh->texture[0] = this->textureFileName;
	mesh->textureRatio[0] = 1.0f;

	PhysXBody* physXBody = &enemy->AddComponent<PhysXBody>(transform);
	physXBody->SetBody(false);
	physXBody->SetShape(BOX);
	physXBody->UpdateBoxDimensions(this->halfExtents);
	physXBody->position = this->physXObjPosition;
	physXBody->shape->setLocalPose(PxTransform(PxVec3(this->physXObjPosition.x, this->physXObjPosition.y, this->physXObjPosition.z)));
	physXBody->GameObject = enemy;

	m_AnimationSystem = &enemy->AddComponent<AnimationSystem>();
	m_AnimationSystem->SetMesh(mesh);
	m_AnimationSystem->LoadAnimationFromFile("Warzombie_Walking.dae");
	m_AnimationSystem->SetCurrentAnimation(1);
	m_EnemyStates[enemy] = EnemyState::ALIVE;

	m_Enemies.push_back(enemy);
}
