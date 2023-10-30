#pragma once
#include "../PandaEngine/GraphicsCommon.h"
#include "../Basic Shader Manager/cShaderManager.h"
#include "MeshManager.h"
#include "cLightManager.h"
#include "cLightHelper.h"
#include "PhysicsManager.h"

class Engine
{
public:
	Engine();
	~Engine();

	bool Initialize();
	void Update();
	void SetShaderPath(std::string filePath);
	void SetModelPath(std::string filePath);
	cMesh* LoadMesh(std::string filePath, std::string friendlyName);
	PhysicsBody* AddPhysicsBody(std::string friendlyMeshName);
	bool LoadDefaultShaders();
	void LoadDefaultLights();
	void TestRun();
	double deltaTime;
	bool IsRunning;
	void ShutDown();
	GLFWwindow* window;
private:
	PhysicsManager* physicsManager;
	cShaderManager* shaderManager;
	MeshManager* meshManager;
	cLightManager* lightManager;
	GLuint shaderProgramID;
	glm::vec3 cameraEye;
	glm::vec3 cameraTarget;
	glm::vec3 upVector;
	double lastTime;
};