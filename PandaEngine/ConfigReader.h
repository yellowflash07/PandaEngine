#pragma once

#include "JsonReader.h"
#include <string>
#include <vector>
#include "cMesh.h"
#include "cLightManager.h"
#include "PhysicsManager.h"

struct UserDef
{
    int windowWidth;
    int windowHeight;
    std::string windowTitle;
};

struct SceneDef
{
    std::vector<cMesh*> meshes;
    std::vector<cLight*> lights;
    std::vector<PhysicsBody*> physicsBodies;
};

struct ConfigDef
{
    UserDef* userDef;
    std::vector<SceneDef*> scenes;
};


class ConfigReader
{
public:
    ConfigReader();
    ~ConfigReader();

    bool LoadConfigFile(ConfigDef& config);
    void WriteConfigFile(ConfigDef& config);
    void SaveScene(SceneDef* scene);
private:
    JsonReader reader;
    const std::string path = "../Assets/Json/";
    const std::string configPath = "config.json";
};

