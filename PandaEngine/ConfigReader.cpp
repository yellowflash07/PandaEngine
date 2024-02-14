#include "ConfigReader.h"

ConfigReader::ConfigReader()
{
}

ConfigReader::~ConfigReader()
{
}

bool ConfigReader::LoadConfigFile(ConfigDef& config)
{
  //  JsonReader reader;
    rapidjson::Document document;
    if (reader.LoadJsonFile((path + configPath).c_str(), document))
    {
        std::cout << "Loaded json file" << std::endl;
    }
    else
    {
        std::cout << "Failed to load json file" << std::endl;
        return 1;
    }

    if (document.HasMember("Config"))
    {
        printf("yea\n");
        rapidjson::Value& userPref = document["Config"]["UserPrefs"];

        rapidjson::Document userPrefDoc;
        std::string userPrefPath = path + userPref.GetString();
        config.userDef = new UserDef();
        if (reader.LoadJsonFile(userPrefPath.c_str(), userPrefDoc))
        {
            if (userPrefDoc.HasMember("UserPrefs"))
            {
                printf("user prefs yea\n");

                config.userDef->windowWidth = userPrefDoc["UserPrefs"]["WindowWidth"].GetInt();
                config.userDef->windowHeight = userPrefDoc["UserPrefs"]["WindowHeight"].GetInt();
                config.userDef->windowTitle = userPrefDoc["UserPrefs"]["WindowTitle"].GetString();

                //config.userDef = userDef;
            }
        }
        else
        {
            printf("Failed to load user prefs\n");
            return 1;
        }

        rapidjson::Value& scenes = document["Config"]["Scenes"];

        for (rapidjson::SizeType i = 0; i < scenes.Size(); i++)
        {
			rapidjson::Document sceneDoc;
            std::string scenePath = path + scenes[i]["SceneData"].GetString();
            if (reader.LoadJsonFile(scenePath.c_str(), sceneDoc))
            {
				SceneDef* sceneDef = new SceneDef();
                if (sceneDoc.HasMember("Scene"))
                {
					rapidjson::Value& meshes = sceneDoc["Scene"]["Meshes"];
                    printf("meshes.Size %d \n", meshes.Size());

                }
            }
            else
            {
				printf("Failed to load scene\n");
				return 1;
			}
        }

    }
}

void ConfigReader::WriteConfigFile(ConfigDef& config)
{
    rapidjson::Document document;
    rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
    rapidjson::Value configValue(rapidjson::kObjectType);
    rapidjson::Value userPrefs(rapidjson::kObjectType);
    userPrefs.AddMember("WindowWidth", config.userDef->windowWidth, allocator);
    userPrefs.AddMember("WindowHeight", config.userDef->windowHeight, allocator);
   // userPrefs.AddMember("WindowTitle", config.userDef->windowTitle, allocator);
    configValue.AddMember("UserPrefs", userPrefs, allocator);
    reader.WriteJsonFile((path + configPath).c_str(), document);
}

void ConfigReader::SaveScene(SceneDef* scene)
{

}
