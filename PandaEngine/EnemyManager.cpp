#include "EnemyManager.h"
#include "Raycast.h"

EnemyManager::EnemyManager(Scene* scene, GameObject* player)
	: m_Scene(scene), m_Player(player)
{
	m_EnemySpawnPoints.push_back(glm::vec3(glm::vec3(-1514, -86.11, 11.56)));
	m_EnemySpawnPoints.push_back(glm::vec3(glm::vec3(-600, -86.11, 2066)));
	m_EnemySpawnPoints.push_back(glm::vec3(glm::vec3(1846, -86.11, 1766)));
	m_EnemySpawnPoints.push_back(glm::vec3(glm::vec3(2081, -86.11, 417)));
	m_EnemySpawnPoints.push_back(glm::vec3(glm::vec3(2081, -86.11, 417)));

}

EnemyManager::~EnemyManager()
{
}

void EnemyManager::Update(float deltaTime)
{
	if (m_Enemies.size() < 5)
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

			PxTransform pxTransform = PxShapeExt::getGlobalPose(*physXBody->shape, *physXBody->body);
			glm::vec3 pos = glm::vec3(pxTransform.p.x, pxTransform.p.y, pxTransform.p.z);
			Raycast raycast(transform->drawPosition, transform->GetForwardVector(), 1000.0f);
			HitResult hitResult;
			if (raycast.RaycastHit(hitResult))
			{
				if (hitResult.hitActor != nullptr)
				{
					GameObject* hitObject = (GameObject*)hitResult.hitActor->GameObject;
					if (hitObject != nullptr)
					{
						if (hitObject->m_Name == "Terrain" && hitResult.distance < 30.0f)
						{
							//printf("Collided with terrain\n");
							continue;
						}
					}
				}
			}

			glm::vec3 direction = playerTransform->drawPosition - transform->drawPosition;
			direction = glm::normalize(direction);
			
			glm::vec3 rotation = glm::eulerAngles(glm::quatLookAt(-direction, glm::vec3(0.0f, 1.0f, 0.0f)));
			transform->eulerRotation = rotation;

			glm::vec3 enemyPosition = direction * speed;
			glm::vec3 flatPosition = glm::vec3(enemyPosition.x, 0.0f, enemyPosition.z);
			//move the enemy towards the target
			transform->drawPosition += flatPosition;
			//physXBody->position += direction * speed;
			physXBody->body->setGlobalPose(PxTransform(PxVec3(transform->drawPosition.x, transform->drawPosition.y, transform->drawPosition.z)));
		}
		else if (m_EnemyStates[enemy] == EnemyState::DEAD)
		{
			if (m_EnemyDeathTimes[enemy] > deathTime)
			{
				//RemoveEnemy(enemy);
				PhysXBody* physXBody = enemy->GetComponent<PhysXBody>();
				physXBody->isTrigger = true;
				physXBody->SetTrigger();
				m_Enemies.erase(std::remove(m_Enemies.begin(), m_Enemies.end(), enemy), m_Enemies.end());
				m_Scene->DestroyGameObject(enemy);
			}
			else
			{
				m_EnemyDeathTimes[enemy] += deltaTime;
			}
		}
	}
}

void EnemyManager::AddEnemy()
{

}

void EnemyManager::RemoveEnemy(GameObject* enemy)
{
	m_EnemyStates[enemy] = EnemyState::DEAD;
	enemy->GetComponent<AnimationSystem>()->SetCurrentAnimation(2);
}


void EnemyManager::CreateEnemy()
{
	GameObject* enemy = m_Scene->CreateGameObject(this->name + std::to_string(currentEnemyCount));
	enemy->tag = "Zombie";
	TransformComponent* transform = enemy->GetComponent<TransformComponent>();	
	transform->drawPosition = m_EnemySpawnPoints[rand() % m_EnemySpawnPoints.size()];
	//transform->drawPosition = m_EnemySpawnPoints[0];
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

	AnimationSystem* m_AnimationSystem = &enemy->AddComponent<AnimationSystem>();
	m_AnimationSystem->SetMesh(mesh);
	m_AnimationSystem->LoadAnimationFromFile("Warzombie_Walking.dae");
	m_AnimationSystem->LoadAnimationFromFile("Zombie Death.dae");
	m_AnimationSystem->SetCurrentAnimation(1);
	m_EnemyStates[enemy] = EnemyState::ALIVE;

	m_Enemies.push_back(enemy);
}
