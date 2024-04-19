#pragma once

#include "Scene.h"
#include <map>
class EnemyManager
{
public:
	EnemyManager(Scene* scene, GameObject* player);
	~EnemyManager();

    enum class EnemyState
    {
		ALIVE,
        DEAD
	};

	void Update(float deltaTime);

	void AddEnemy();

	void RemoveEnemy(GameObject* enemy);	

    void CreateEnemy();

	int m_EnemyCount = 5;
    Scene* m_Scene;
private:
	std::vector<GameObject*> m_Enemies;
    int currentEnemyCount = 0;
    AnimationSystem* m_AnimationSystem;
    std::map<GameObject*, EnemyState> m_EnemyStates;

    std::vector<glm::vec3> m_EnemySpawnPoints;

    GameObject* m_Player;
    float speed = 0.5f;
    std::string name = "Zombie";
    std::string fileName = "Warzombie F Pedroso.dae";
    glm::vec3 position = glm::vec3(-1514.6943359375, -86.11502075195313, 11.56004524230957);
    glm::vec3 rotation = glm::vec3(0.0, -1.4678195714950562, 0.0);
    glm::vec3 scale = glm::vec3(0.44159820675849915, 0.44159814715385437, 0.44159820675849915);
    std::string meshFileName = "Warzombie F Pedroso.dae";
    std::string textureFileName = "world_war_zombie_diffuse.png";
    std::string normalMapFileName = "world_war_zombie_normal.png";
    glm::vec3 halfExtents = glm::vec3(7.671999931335449,
        39.27199935913086,
        7.671999931335449);
    glm::vec3 physXObjPosition = glm::vec3(0.0, 43.70000076293945, 0.0);
	
};

