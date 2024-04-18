#pragma once
#include "../PandaEngine/GraphicsCommon.h"
#include "../Basic Shader Manager/cShaderManager.h"
#include "MeshManager.h"
#include "cLightManager.h"
#include "cLightHelper.h"
#include "PhysicsManager.h"
#include "Camera.h"
#include "AudioManager.h"
#include "AssetLibrary.h"
#include "RenderTexture.h"
#include "Scene.h"
#include "SceneSaver.h"

class Engine
{
public:
	Engine();
	~Engine();

	bool Initialize();
	void Update();
	void SetShaderPath(std::string filePath);
	void SetModelPath(std::string filePath);
	void SetAudioPath(std::string filePath);
	cMesh* LoadMesh(std::string filePath, std::string friendlyName, bool dontDraw = false);
	PhysicsBody* AddPhysicsBody(std::string friendlyMeshName);
	bool LoadDefaultShaders();
	void LoadDefaultLights();
	void LoadSave(std::string sceneName);
	double deltaTime;
	bool IsRunning;
	void ShutDown();
	void AddScene(Scene* scene);
	RenderTexture* CreateRenderTexture(Camera* camera, std::vector<cMesh*> offScreenMesh, int width = 1920, int height = 1080);
	GLFWwindow* window;
	cLightManager* lightManager;
	MeshManager* meshManager;
	cAudioManager* audioManager;
	PhysicsManager* physicsManager;
	GLuint shaderProgramID;
	int currentScene = 0;
	Scene* GetCurrentScene();
	void BeginRender();
	void EndRender();
private:
	//cShaderManager* shaderManager;
	glm::vec3 cameraEye;
	glm::vec3 cameraTarget;
	glm::vec3 upVector;
	double lastTime;
	int frameCount;
	AssetLibrary assetLib;
	std::vector<RenderTexture*> renderTextures;
	std::vector<Scene*> scenes;
	SceneSaver sceneSaver;
	bool saveFile = false;
	bool loadFile = false;
	std::string currentFile;
	//Camera* camera;
};