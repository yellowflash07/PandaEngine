#pragma once
#include <vector>
#include "Scene.h"
#include "JsonReader.h"
#include <string>
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
	glm::vec3 halfExtents;
	float radius;
};

struct AnimationConfig
{

};

struct GameObjectConfig
{
	std::string name;
	TransformConfig transform;
	MeshConfig mesh;
 	LightConfig light;
	AnimationConfig animation;
	PhysXConfig physXObj;
	std::vector<GameObjectConfig> children;
};

struct SceneConfig
{
	std::string sceneName;
	std::vector<GameObjectConfig> gameObjects;
};


class SceneSaver
{
public:
	SceneSaver();
	~SceneSaver();

	//void SaveMeshes(std::vector<cMesh*> meshes);
	//std::vector<cMesh*> LoadMeshes();

	void SaveScene(Scene* scene);
	Scene* LoadScene(std::string sceneName);
private:
	JsonReader* jsonReader;

	void GetGameObjectConifg(GameObject* go, GameObjectConfig& config);
	void SaveGameObject(GameObjectConfig go, rapidjson::Value& gameObjectValue, rapidjson::Document& document);

	GameObject* LoadGameObject(rapidjson::Value& gameObjectValue, Scene* scene);
};