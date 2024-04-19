#pragma once
#include <vector>
#include "Scene.h"
#include "JsonReader.h"
#include "MeshManager.h"
#include <string>
#include <functional>
struct TransformConfig
{
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;
};

struct MeshConfig
{
	std::string fileName;
	std::string friendlyName;
	bool bIsWireframe;
	bool bDoNotLight;
	bool bIsVisible;
	bool bUseDebugColours;
	glm::vec4 wholeObjectDebugColourRGBA;
	glm::vec4 color;
	bool isSkyBox;
	bool hasVertexColors;
	bool isReflective;
	bool useBone;
	std::string texture[4];
	float textureRatio[4];
	std::string maskTexture;
	std::string normalMap;
	bool enableShadow;
	bool isDynamicLOD;
	float transperancy = 1.0f;
	glm::vec2 UV_Offset = glm::vec2(0.0f);
	glm::vec2 UV_Tiling = glm::vec2(1.0f);
};

struct LightConfig
{
	int index;
	glm::vec3 position;
	glm::vec3 diffuse;
	glm::vec3 specular;
	glm::vec3 atten;
	glm::vec3 direction;
	glm::vec3 param1;
	glm::vec3 param2;
};

struct PhysXConfig
{
	ColliderType type = NONE;
	bool isDynamic;
	bool isTrigger;
	glm::vec3 halfExtents = glm::vec3(0.0f);
	float radius = 0.0f;
};

struct AnimationConfig
{

};

struct CharacterControllerConfig
{
	float height = 0.0f;
	float radius = 0.0f;
	glm::vec3 position;
	float stepOffset = 0.0f;
	float slopeLimit = 0.0f;
};

struct GameObjectConfig
{
	std::string name;
	TransformConfig transform;
	MeshConfig mesh;
 	LightConfig light;
	AnimationConfig animation;
	PhysXConfig physXObj;
	CharacterControllerConfig characterController;
	std::vector<GameObjectConfig> children;
	bool loadAsync;
};

struct SceneConfig
{
	std::string sceneName;
	std::vector<GameObjectConfig> gameObjects;
};

//typedef std::function<void(GameObject*)> OnGameObjectLoadCallBack;

class SceneSaver
{
public:
	SceneSaver();
	~SceneSaver();

	//void SaveMeshes(std::vector<cMesh*> meshes);
	//std::vector<cMesh*> LoadMeshes();

	void SaveScene(Scene* scene, std::string fileName);
	Scene* LoadScene(std::string sceneName);

	//void LoadGameObjectAsync(GameObjectConfig go, Scene* scene, OnGameObjectLoadCallBack callback);
	MeshManager* meshManager;
private:
	JsonReader* jsonReader;

	void GetGameObjectConifg(GameObject* go, GameObjectConfig& config);
	void SaveGameObject(GameObjectConfig go, rapidjson::Value& gameObjectValue, rapidjson::Document& document);

	void GetLoadGameObjectConfig(rapidjson::Value& gameObjectValue, GameObjectConfig& config);
	GameObject* LoadGameObject(GameObjectConfig& config, Scene* scene);

	cShaderManager* shaderManager;

	GameObject* LoadGameObjectAsync(cMesh* mesh, GameObjectConfig config, Scene* scene);
};