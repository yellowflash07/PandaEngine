#include "SceneSaver.h"
#include <fstream>
#include <sstream>

SceneSaver::SceneSaver()
{
    jsonReader = new JsonReader();
    shaderManager = cShaderManager::getInstance();
}

SceneSaver::~SceneSaver()
{
}

//void SceneSaver::SaveMeshes(std::vector<cMesh*> meshes)
//{
//    std::ofstream file("mesh_values.txt");
//
//    if (file.is_open()) 
//    {
//        for (cMesh* mesh : meshes) 
//        {
//            if (mesh->isSkyBox)
//            {
//                continue;
//            }
//
//            file << "Mesh Name:" << mesh->meshName << "\n";
//            file << "Friendly Name:" << mesh->friendlyName << "\n";
//            file << "Position:" << mesh->transform.drawPosition.x << " " << mesh->transform.drawPosition.y << " " << mesh->transform.drawPosition.z << "\n";
//            file << "Rotation:" << mesh->transform.eulerRotation.x << " " << mesh->transform.eulerRotation.y << " " << mesh->transform.eulerRotation.z << "\n";
//            file << "Scale:" << mesh->transform.drawScale.x << " " << mesh->transform.drawScale.y << " " << mesh->transform.drawScale.z << "\n";
//            file << "Color:" << mesh->color.x << " " << mesh->color.y << " " << mesh->color.z << " " << mesh->color.w << "\n";
//            file << "Transparency:" << mesh->transperancy << "\n";
//
//            for (size_t i = 0; i < mesh->NUM_OF_TEXTURES; i++)
//            {
//                file << "Texture" + std::to_string(i) + ":" << mesh->texture[i] << "\n";
//                file << "TextureRatio" + std::to_string(i) + ":" << mesh->textureRatio[i] << "\n";
//            }
//
//            file << "Mask Texture:" << mesh->maskTexture << "\n";
//
//            // Separate meshes with an empty line
//            file << "\n";
//        }
//
//        file.close();
//    }
//}
//
//std::vector<cMesh*> SceneSaver::LoadMeshes()
//{
//    std::vector<cMesh*> loadedMeshes;
//    std::ifstream file("mesh_values.txt");
//
//    // Function to load values from a text file
//    if (file.is_open()) {
//        std::string line;
//        bool readMeshData = false;
//        cMesh currentMesh;
//
//        while (std::getline(file, line)) 
//        {
//            std::istringstream iss(line);
//            std::string token;
//
//            if (std::getline(iss, token, ':')) 
//            {
//                if (token == "Mesh Name") 
//                {
//                    // If we've already read mesh data for a previous mesh, add it to the vector
//                    if (readMeshData) 
//                    {
//                        cMesh* newMesh = new cMesh;
//                        newMesh->meshName = currentMesh.meshName;
//                        newMesh->friendlyName = currentMesh.friendlyName;
//                        newMesh->transform.drawPosition = currentMesh.transform.drawPosition;
//                        newMesh->transform.eulerRotation = currentMesh.transform.eulerRotation;
//                        newMesh->transform.drawScale = currentMesh.transform.drawScale;
//                        newMesh->color = currentMesh.color;
//                        newMesh->transperancy = currentMesh.transperancy;
//                        newMesh->texture[0] = currentMesh.texture[0];
//                        newMesh->texture[1] = currentMesh.texture[1];
//                        newMesh->texture[2] = currentMesh.texture[2];
//                        newMesh->texture[3] = currentMesh.texture[3];
//                        newMesh->textureRatio[0] = currentMesh.textureRatio[0];
//                        newMesh->textureRatio[1] = currentMesh.textureRatio[1];
//                        newMesh->textureRatio[2] = currentMesh.textureRatio[2];
//                        newMesh->textureRatio[3] = currentMesh.textureRatio[3];
//                        newMesh->maskTexture = currentMesh.maskTexture;
//                        loadedMeshes.push_back(newMesh);
//                    }
//
//                    std::getline(iss, currentMesh.meshName);
//                }
//                else if (token == "Friendly Name") 
//                {
//                    std::getline(iss, currentMesh.friendlyName);
//                }
//                else if (token == "Position")
//                {
//                    iss >> currentMesh.transform.drawPosition.x >> currentMesh.transform.drawPosition.y >> currentMesh.transform.drawPosition.z;
//                }
//                else if (token == "Rotation") 
//                {
//                    iss >> currentMesh.transform.eulerRotation.x >> currentMesh.transform.eulerRotation.y >> currentMesh.transform.eulerRotation.z;
//                }
//                else if (token == "Scale") 
//                {
//                    iss >> currentMesh.transform.drawScale.x >> currentMesh.transform.drawScale.y >> currentMesh.transform.drawScale.z;
//                }
//                else if (token == "Color")
//                {
//					iss >> currentMesh.color.x >> currentMesh.color.y >> currentMesh.color.z >> currentMesh.color.w;
//				}
//                else if (token == "Transparency")
//                {
//                    iss >> currentMesh.transperancy;
//                }
//                else if (token == "Texture0")
//                {
//					std::getline(iss, currentMesh.texture[0]);
//				}
//                else if (token == "Texture1")
//                {
//					std::getline(iss, currentMesh.texture[1]);
//				}
//                else if (token == "Texture2")
//                {
//					std::getline(iss, currentMesh.texture[2]);
//				}
//                else if (token == "Texture3")
//                {
//					std::getline(iss, currentMesh.texture[3]);
//				}
//                else if (token == "TextureRatio0")
//                {
//					iss >> currentMesh.textureRatio[0];
//				}
//                else if (token == "TextureRatio1")
//                {
//					iss >> currentMesh.textureRatio[1];
//				}
//                else if (token == "TextureRatio2")
//                {
//					iss >> currentMesh.textureRatio[2];
//				}
//                else if (token == "TextureRatio3")
//                {
//					iss >> currentMesh.textureRatio[3];
//				}
//                else if (token == "Mask Texture")
//                {
//					std::getline(iss, currentMesh.maskTexture);
//				}
//            }
//            else if (line.empty()) 
//            {
//                // An empty line indicates the end of a mesh's data
//                readMeshData = true;
//            }
//        }
//
//        // Add the last mesh to the vector
//        if (readMeshData) 
//        {
//            cMesh* newMesh = new cMesh;
//            newMesh->meshName = currentMesh.meshName;
//            newMesh->friendlyName = currentMesh.friendlyName;
//            newMesh->transform.drawPosition = currentMesh.transform.drawPosition;
//            newMesh->transform.eulerRotation = currentMesh.transform.eulerRotation;
//            newMesh->transform.drawScale = currentMesh.transform.drawScale;
//            newMesh->color = currentMesh.color;
//            newMesh->transperancy = currentMesh.transperancy;
//            newMesh->texture[0] = currentMesh.texture[0];
//            newMesh->texture[1] = currentMesh.texture[1];
//            newMesh->texture[2] = currentMesh.texture[2];
//            newMesh->texture[3] = currentMesh.texture[3];
//            newMesh->textureRatio[0] = currentMesh.textureRatio[0];
//            newMesh->textureRatio[1] = currentMesh.textureRatio[1];
//            newMesh->textureRatio[2] = currentMesh.textureRatio[2];
//            newMesh->textureRatio[3] = currentMesh.textureRatio[3];
//            newMesh->maskTexture = currentMesh.maskTexture;
//            loadedMeshes.push_back(newMesh);
//        }
//
//        file.close();
//    }
//    
//	return loadedMeshes;
//}


void SceneSaver::SaveScene(Scene* scene, std::string fileName)
{

    SceneConfig sceneConfig;

    sceneConfig.sceneName = scene->name;

    for (size_t i = 0; i < scene->GetGameObjects().size(); i++)
    {
      
        GameObject* go = scene->GetGameObjects()[i];    

        GameObjectConfig gameObjectConfig;
        GetGameObjectConifg(go, gameObjectConfig);

        sceneConfig.gameObjects.push_back(gameObjectConfig);
    }

    rapidjson::Document document;
    document.SetObject();
    rapidjson::Value sceneName;
    sceneName.SetString(sceneConfig.sceneName.c_str(), sceneConfig.sceneName.size(), document.GetAllocator());
    document.AddMember("sceneName", sceneName, document.GetAllocator());

    rapidjson::Value gameObjects(rapidjson::kArrayType);
    for (size_t i = 0; i < sceneConfig.gameObjects.size(); i++)
    {

		rapidjson::Value gameObject(rapidjson::kObjectType);

        SaveGameObject(sceneConfig.gameObjects[i], gameObject, document);

        gameObjects.PushBack(gameObject, document.GetAllocator());
    }

    document.AddMember("gameObjects", gameObjects, document.GetAllocator());

    std::string filePath = "../Assets/Json/" + fileName;

    jsonReader->WriteJsonFile(filePath.c_str(), document);

}

Scene* SceneSaver::LoadScene(std::string sceneFile)
{
    rapidjson::Document document;
    std::string filePath = "../Assets/Json/" + sceneFile;
    if (!jsonReader->LoadJsonFile(filePath.c_str(), document))
    {
        std::cerr << "Failed to load scene file\n";
		return nullptr;
	}

   
	std::string sceneName = document["sceneName"].GetString();
    Scene* scene = new Scene(sceneName);

    //TODO: thread?
	rapidjson::Value& gameObjects = document["gameObjects"];
    std::vector<GameObjectConfig> gameObjectConfigs;
    for (rapidjson::SizeType i = 0; i < gameObjects.Size(); i++)
    {		
        GameObjectConfig gameObjectConfig;
        GetLoadGameObjectConfig(gameObjects[i], gameObjectConfig);
        gameObjectConfigs.push_back(gameObjectConfig);
		//GameObject* go = LoadGameObject(gameObjects[i], scene);
        //scene->AddGameObject(go);
    }



    for (size_t i = 0; i < gameObjectConfigs.size(); i++)
    {
        GameObjectConfig config = gameObjectConfigs[i];
       // GameObject* go = LoadGameObject(config, scene);
       // scene->AddGameObject(go);
        if (config.loadAsync)
        {
            
            if (!config.mesh.fileName.empty())
            {
                meshManager->LoadMeshAsync(config.mesh.fileName, config.mesh.friendlyName, shaderManager->getIDFromFriendlyName("shader01"),
                [config, scene, this](cMesh* mesh)
                {
                    GameObject* go =  LoadGameObjectAsync(mesh, config, scene);
                    scene->AddGameObject(go);
			    });
            }

		}
        else
        {
             GameObject* go = LoadGameObject(config, scene);
             scene->AddGameObject(go);
        }
	}

    return scene;

}

void SceneSaver::GetGameObjectConifg(GameObject* go, GameObjectConfig& gameObjectConfig)
{
    gameObjectConfig.name = go->m_Name;

    gameObjectConfig.loadAsync = go->LoadAsync;

    TransformConfig transformConfig = TransformConfig();

    transformConfig.position = go->GetComponent<TransformComponent>()->drawPosition;
    transformConfig.rotation = go->GetComponent<TransformComponent>()->eulerRotation;
    transformConfig.scale = go->GetComponent<TransformComponent>()->drawScale;

    gameObjectConfig.transform = transformConfig;

    cMesh* mesh = go->GetComponent<cMesh>();
    if (mesh != nullptr)
    {
        MeshConfig meshConfig;
        meshConfig.fileName = mesh->meshName;
        meshConfig.friendlyName = mesh->friendlyName;
        meshConfig.bIsWireframe = mesh->bIsWireframe;
        meshConfig.bDoNotLight = mesh->bDoNotLight;
        meshConfig.bIsVisible = mesh->bIsVisible;
        meshConfig.bUseDebugColours = mesh->bUseDebugColours;
        meshConfig.wholeObjectDebugColourRGBA = mesh->wholeObjectDebugColourRGBA;
        meshConfig.color = mesh->color;
        meshConfig.isSkyBox = mesh->isSkyBox;
        meshConfig.hasVertexColors = mesh->hasVertexColors;
        meshConfig.isReflective = mesh->isReflective;
        meshConfig.useBone = mesh->useBone;
        meshConfig.texture[0] = mesh->texture[0];
        meshConfig.texture[1] = mesh->texture[1];
        meshConfig.texture[2] = mesh->texture[2];
        meshConfig.texture[3] = mesh->texture[3];
        meshConfig.textureRatio[0] = mesh->textureRatio[0];
        meshConfig.textureRatio[1] = mesh->textureRatio[1];
        meshConfig.textureRatio[2] = mesh->textureRatio[2];
        meshConfig.textureRatio[3] = mesh->textureRatio[3];
        meshConfig.maskTexture = mesh->maskTexture;
        meshConfig.normalMap = mesh->normalMap;
        meshConfig.enableShadow = mesh->enableShadow;
        meshConfig.isDynamicLOD = mesh->dynamicLOD;
        meshConfig.UV_Offset = mesh->UV_Offset;
        meshConfig.UV_Tiling = mesh->UV_Tiling;
        meshConfig.transperancy = mesh->transperancy;
        gameObjectConfig.mesh = meshConfig;
    }


    cLight* light = go->GetComponent<cLight>();
    LightConfig lightConfig;
    if (light != nullptr)
    {
       
        lightConfig.index = light->index;
        lightConfig.position = light->position;
        lightConfig.diffuse = light->diffuse;
        lightConfig.specular = light->specular;
        lightConfig.atten = light->atten;
        lightConfig.direction = light->direction;
        lightConfig.param1 = light->param1;
        lightConfig.param2 = light->param2;       
    }
    else
    {
        lightConfig.index = -1;
    }
    gameObjectConfig.light = lightConfig;
    PhysXBody* physXBody = go->GetComponent<PhysXBody>();
    if (physXBody != nullptr)
    {
		PhysXConfig physXConfig;
		physXConfig.type = physXBody->type;

        switch (physXConfig.type)
        {
            case BOX:
                physXConfig.halfExtents = physXBody->halfExtents;
			    break;
			case SPHERE:
				physXConfig.radius = physXBody->radius;
                break;
        }

		physXConfig.isDynamic = physXBody->isDynamic;
        physXConfig.isTrigger = physXBody->isTrigger;
		gameObjectConfig.physXObj = physXConfig;
	}

    CharacterController* characterController = go->GetComponent<CharacterController>();
    if (characterController != nullptr)
    {
        CharacterControllerConfig characterControllerConfig;
        characterControllerConfig.height = characterController->height;
        characterControllerConfig.radius = characterController->radius;
        characterControllerConfig.position = characterController->position;
        characterControllerConfig.stepOffset = characterController->stepOffset;
        characterControllerConfig.slopeLimit = characterController->slopeLimit;
        gameObjectConfig.characterController = characterControllerConfig;
    }
    else
    {
		CharacterControllerConfig characterControllerConfig;
		characterControllerConfig.height = 0;
		characterControllerConfig.radius = 0;
		characterControllerConfig.position = glm::vec3(0);
		gameObjectConfig.characterController = characterControllerConfig;
    }


    gameObjectConfig.children.clear();
    for (GameObject* child : go->m_Children)
    {
		GameObjectConfig childConfig;
		GetGameObjectConifg(child, childConfig);
		gameObjectConfig.children.push_back(childConfig);
	}
}

void SceneSaver::SaveGameObject(GameObjectConfig go, rapidjson::Value& gameObjectValue, rapidjson::Document& jsonDocument)
{
    rapidjson::Value name;
    name.SetString(go.name.c_str(), go.name.size(), jsonDocument.GetAllocator());

    gameObjectValue.AddMember("name", name, jsonDocument.GetAllocator());

    rapidjson::Value loadAsync(go.loadAsync);
    gameObjectValue.AddMember("loadAsync", loadAsync, jsonDocument.GetAllocator());

    rapidjson::Value transform(rapidjson::kObjectType);
    rapidjson::Value position(rapidjson::kArrayType);
    position.PushBack(go.transform.position.x, jsonDocument.GetAllocator());
    position.PushBack(go.transform.position.y, jsonDocument.GetAllocator());
    position.PushBack(go.transform.position.z, jsonDocument.GetAllocator());
    transform.AddMember("position", position, jsonDocument.GetAllocator());

    rapidjson::Value rotation(rapidjson::kArrayType);
    rotation.PushBack(go.transform.rotation.x, jsonDocument.GetAllocator());
    rotation.PushBack(go.transform.rotation.y, jsonDocument.GetAllocator());
    rotation.PushBack(go.transform.rotation.z, jsonDocument.GetAllocator());
    transform.AddMember("rotation", rotation, jsonDocument.GetAllocator());

    rapidjson::Value scale(rapidjson::kArrayType);
    scale.PushBack(go.transform.scale.x, jsonDocument.GetAllocator());
    scale.PushBack(go.transform.scale.y, jsonDocument.GetAllocator());
    scale.PushBack(go.transform.scale.z, jsonDocument.GetAllocator());

    transform.AddMember("scale", scale, jsonDocument.GetAllocator());
    gameObjectValue.AddMember("transform", transform, jsonDocument.GetAllocator());


    if (!go.mesh.fileName.empty())
    {
        rapidjson::Value mesh(rapidjson::kObjectType);
        rapidjson::Value fileName;
        fileName.SetString(go.mesh.fileName.c_str(), go.mesh.fileName.size(), jsonDocument.GetAllocator());
        mesh.AddMember("fileName", fileName, jsonDocument.GetAllocator());

        rapidjson::Value friendlyName;
        friendlyName.SetString(go.mesh.friendlyName.c_str(), go.mesh.friendlyName.size(), jsonDocument.GetAllocator());
        mesh.AddMember("friendlyName", friendlyName, jsonDocument.GetAllocator());
        mesh.AddMember("bIsWireframe", go.mesh.bIsWireframe, jsonDocument.GetAllocator());
        mesh.AddMember("bDoNotLight", go.mesh.bDoNotLight, jsonDocument.GetAllocator());
        mesh.AddMember("bIsVisible", go.mesh.bIsVisible, jsonDocument.GetAllocator());
        mesh.AddMember("bUseDebugColours", go.mesh.bUseDebugColours, jsonDocument.GetAllocator());

        rapidjson::Value wholeObjectDebugColourRGBA(rapidjson::kArrayType);
        wholeObjectDebugColourRGBA.PushBack(go.mesh.wholeObjectDebugColourRGBA.x, jsonDocument.GetAllocator());
        wholeObjectDebugColourRGBA.PushBack(go.mesh.wholeObjectDebugColourRGBA.y, jsonDocument.GetAllocator());
        wholeObjectDebugColourRGBA.PushBack(go.mesh.wholeObjectDebugColourRGBA.z, jsonDocument.GetAllocator());
        wholeObjectDebugColourRGBA.PushBack(go.mesh.wholeObjectDebugColourRGBA.w, jsonDocument.GetAllocator());
        mesh.AddMember("wholeObjectDebugColourRGBA", wholeObjectDebugColourRGBA, jsonDocument.GetAllocator());

        rapidjson::Value color(rapidjson::kArrayType);
        color.PushBack(go.mesh.color.x, jsonDocument.GetAllocator());
        color.PushBack(go.mesh.color.y, jsonDocument.GetAllocator());
        color.PushBack(go.mesh.color.z, jsonDocument.GetAllocator());
        color.PushBack(go.mesh.color.w, jsonDocument.GetAllocator());
        mesh.AddMember("color", color, jsonDocument.GetAllocator());

        mesh.AddMember("isSkyBox", go.mesh.isSkyBox, jsonDocument.GetAllocator());
        mesh.AddMember("hasVertexColors", go.mesh.hasVertexColors, jsonDocument.GetAllocator());
        mesh.AddMember("isReflective", go.mesh.isReflective, jsonDocument.GetAllocator());
        mesh.AddMember("useBone", go.mesh.useBone, jsonDocument.GetAllocator());

        rapidjson::Value texture(rapidjson::kArrayType);
        for (size_t k = 0; k < 4; k++)
        {
            rapidjson::Value textureName;
            textureName.SetString(go.mesh.texture[k].c_str(), go.mesh.texture[k].size(), jsonDocument.GetAllocator());
            texture.PushBack(textureName, jsonDocument.GetAllocator());
        }

        mesh.AddMember("texture", texture, jsonDocument.GetAllocator());

        rapidjson::Value textureRatio(rapidjson::kArrayType);
        for (size_t k = 0; k < 4; k++)
        {
            textureRatio.PushBack(go.mesh.textureRatio[k], jsonDocument.GetAllocator());
        }

        mesh.AddMember("textureRatio", textureRatio, jsonDocument.GetAllocator());

        rapidjson::Value maskTexture;
        maskTexture.SetString(go.mesh.maskTexture.c_str(), go.mesh.maskTexture.size(), jsonDocument.GetAllocator());
        mesh.AddMember("maskTexture", maskTexture, jsonDocument.GetAllocator());

        rapidjson::Value normalMap;
        normalMap.SetString(go.mesh.normalMap.c_str(), go.mesh.normalMap.size(), jsonDocument.GetAllocator());
        mesh.AddMember("normalMap", normalMap, jsonDocument.GetAllocator());

        rapidjson::Value UV_Offset(rapidjson::kArrayType);
        UV_Offset.PushBack(go.mesh.UV_Offset.x, jsonDocument.GetAllocator());
        UV_Offset.PushBack(go.mesh.UV_Offset.y, jsonDocument.GetAllocator());
        mesh.AddMember("UV_Offset", UV_Offset, jsonDocument.GetAllocator());

        rapidjson::Value UV_Tiling(rapidjson::kArrayType);
        UV_Tiling.PushBack(go.mesh.UV_Tiling.x, jsonDocument.GetAllocator());
        UV_Tiling.PushBack(go.mesh.UV_Tiling.y, jsonDocument.GetAllocator());
        mesh.AddMember("UV_Tiling", UV_Tiling, jsonDocument.GetAllocator());

        mesh.AddMember("enableShadow", go.mesh.enableShadow, jsonDocument.GetAllocator());
        mesh.AddMember("isDynamicLOD", go.mesh.isDynamicLOD, jsonDocument.GetAllocator());

        mesh.AddMember("transperancy", go.mesh.transperancy, jsonDocument.GetAllocator());

        gameObjectValue.AddMember("mesh", mesh, jsonDocument.GetAllocator());
    }

    if (go.light.index > -1)
    {
        rapidjson::Value light(rapidjson::kObjectType);
        light.AddMember("index", go.light.index, jsonDocument.GetAllocator());

        rapidjson::Value position(rapidjson::kArrayType);
        position.PushBack(go.light.position.x, jsonDocument.GetAllocator());
        position.PushBack(go.light.position.y, jsonDocument.GetAllocator());
        position.PushBack(go.light.position.z, jsonDocument.GetAllocator());
        light.AddMember("position", position, jsonDocument.GetAllocator());

        rapidjson::Value diffuse(rapidjson::kArrayType);
        diffuse.PushBack(go.light.diffuse.x, jsonDocument.GetAllocator());
        diffuse.PushBack(go.light.diffuse.y, jsonDocument.GetAllocator());
        diffuse.PushBack(go.light.diffuse.z, jsonDocument.GetAllocator());
        light.AddMember("diffuse", diffuse, jsonDocument.GetAllocator());

        rapidjson::Value specular(rapidjson::kArrayType);
        specular.PushBack(go.light.specular.x, jsonDocument.GetAllocator());
        specular.PushBack(go.light.specular.y, jsonDocument.GetAllocator());
        specular.PushBack(go.light.specular.z, jsonDocument.GetAllocator());
        light.AddMember("specular", specular, jsonDocument.GetAllocator());

        rapidjson::Value atten(rapidjson::kArrayType);
        atten.PushBack(go.light.atten.x, jsonDocument.GetAllocator());
        atten.PushBack(go.light.atten.y, jsonDocument.GetAllocator());
        atten.PushBack(go.light.atten.z, jsonDocument.GetAllocator());
        light.AddMember("atten", atten, jsonDocument.GetAllocator());

        rapidjson::Value direction(rapidjson::kArrayType);
        direction.PushBack(go.light.direction.x, jsonDocument.GetAllocator());
        direction.PushBack(go.light.direction.y, jsonDocument.GetAllocator());
        direction.PushBack(go.light.direction.z, jsonDocument.GetAllocator());
        light.AddMember("direction", direction, jsonDocument.GetAllocator());

        rapidjson::Value param1(rapidjson::kArrayType);
        param1.PushBack(go.light.param1.x, jsonDocument.GetAllocator());
        param1.PushBack(go.light.param1.y, jsonDocument.GetAllocator());
        param1.PushBack(go.light.param1.z, jsonDocument.GetAllocator());
        light.AddMember("param1", param1, jsonDocument.GetAllocator());

        rapidjson::Value param2(rapidjson::kArrayType);
        param2.PushBack(go.light.param2.x, jsonDocument.GetAllocator());
        light.AddMember("param2", param2, jsonDocument.GetAllocator());

        gameObjectValue.AddMember("light", light, jsonDocument.GetAllocator());

    }

    if (go.physXObj.type != NONE)
    {
		rapidjson::Value physXObj(rapidjson::kObjectType);
		physXObj.AddMember("type", go.physXObj.type, jsonDocument.GetAllocator());

        rapidjson::Value halfExtents(rapidjson::kArrayType);

        switch (go.physXObj.type)
        {
            case BOX:
                halfExtents.PushBack(go.physXObj.halfExtents.x, jsonDocument.GetAllocator());
				halfExtents.PushBack(go.physXObj.halfExtents.y, jsonDocument.GetAllocator());
                halfExtents.PushBack(go.physXObj.halfExtents.z, jsonDocument.GetAllocator());
                physXObj.AddMember("halfExtents", halfExtents, jsonDocument.GetAllocator());
                break;
            case SPHERE:                
                physXObj.AddMember("radius", go.physXObj.radius, jsonDocument.GetAllocator());
                break;
        }

		physXObj.AddMember("isDynamic", go.physXObj.isDynamic, jsonDocument.GetAllocator());
		physXObj.AddMember("isTrigger", go.physXObj.isTrigger, jsonDocument.GetAllocator());



        gameObjectValue.AddMember("physXObj", physXObj, jsonDocument.GetAllocator());
	}

    if (go.characterController.height > 0)
    {
        rapidjson::Value characterController(rapidjson::kObjectType);
        characterController.AddMember("height", go.characterController.height, jsonDocument.GetAllocator());
		characterController.AddMember("radius", go.characterController.radius, jsonDocument.GetAllocator());

		rapidjson::Value position(rapidjson::kArrayType);
		position.PushBack(go.characterController.position.x, jsonDocument.GetAllocator());
		position.PushBack(go.characterController.position.y, jsonDocument.GetAllocator());
		position.PushBack(go.characterController.position.z, jsonDocument.GetAllocator());
		characterController.AddMember("position", position, jsonDocument.GetAllocator());

        characterController.AddMember("stepOffset", go.characterController.stepOffset, jsonDocument.GetAllocator());
        characterController.AddMember("slopeLimit", go.characterController.slopeLimit, jsonDocument.GetAllocator());

		gameObjectValue.AddMember("characterController", characterController, jsonDocument.GetAllocator());
    }



    rapidjson::Value children(rapidjson::kArrayType);
    for (size_t i = 0; i < go.children.size(); i++)
    {
		rapidjson::Value child(rapidjson::kObjectType);
		SaveGameObject(go.children[i], child, jsonDocument);
		children.PushBack(child, jsonDocument.GetAllocator());
	}

	gameObjectValue.AddMember("children", children, jsonDocument.GetAllocator());
}

void SceneSaver::GetLoadGameObjectConfig(rapidjson::Value& gameObject, GameObjectConfig& gameObjectConfig)
{
	gameObjectConfig.name = gameObject["name"].GetString();

    if (gameObject.HasMember("loadAsync"))
    {
        gameObjectConfig.loadAsync = gameObject["loadAsync"].GetBool();
	}
    else
    {
		gameObjectConfig.loadAsync = false;
    }
 //   gameObjectConfig.loadAsync = gameObject["loadAsync"].GetBool();

	TransformConfig transformConfig = TransformConfig();
	const rapidjson::Value& transform = gameObject["transform"];
	transformConfig.position = glm::vec3(transform["position"][0].GetFloat(), transform["position"][1].GetFloat(), transform["position"][2].GetFloat());
	transformConfig.rotation = glm::vec3(transform["rotation"][0].GetFloat(), transform["rotation"][1].GetFloat(), transform["rotation"][2].GetFloat());
	transformConfig.scale = glm::vec3(transform["scale"][0].GetFloat(), transform["scale"][1].GetFloat(), transform["scale"][2].GetFloat());

	gameObjectConfig.transform = transformConfig;

    if (gameObject.HasMember("mesh"))
    {
		const rapidjson::Value& mesh = gameObject["mesh"];
		MeshConfig meshConfig;
		meshConfig.fileName = mesh["fileName"].GetString();
		meshConfig.friendlyName = mesh["friendlyName"].GetString();
		meshConfig.bIsWireframe = mesh["bIsWireframe"].GetBool();
		meshConfig.bDoNotLight = mesh["bDoNotLight"].GetBool();
		meshConfig.bIsVisible = mesh["bIsVisible"].GetBool();
		meshConfig.bUseDebugColours = mesh["bUseDebugColours"].GetBool();

		const rapidjson::Value& wholeObjectDebugColourRGBA = mesh["wholeObjectDebugColourRGBA"];
		meshConfig.wholeObjectDebugColourRGBA = glm::vec4(wholeObjectDebugColourRGBA[0].GetFloat(), wholeObjectDebugColourRGBA[1].GetFloat(), wholeObjectDebugColourRGBA[2].GetFloat(), wholeObjectDebugColourRGBA[3].GetFloat());

		const rapidjson::Value& color = mesh["color"];
		meshConfig.color = glm::vec4(color[0].GetFloat(), color[1].GetFloat(), color[2].GetFloat(), color[3].GetFloat());

		meshConfig.isSkyBox = mesh["isSkyBox"].GetBool();
		meshConfig.hasVertexColors = mesh["hasVertexColors"].GetBool();
		meshConfig.isReflective = mesh["isReflective"].GetBool();
		meshConfig.useBone = mesh["useBone"].GetBool();

        const rapidjson::Value& texture = mesh["texture"];
        for (rapidjson::SizeType k = 0; k < texture.Size(); k++)
        {
            meshConfig.texture[k] = texture[k].GetString();
        }

        const rapidjson::Value& textureRatio = mesh["textureRatio"];
        for (rapidjson::SizeType k = 0; k < textureRatio.Size(); k++)
        {
            meshConfig.textureRatio[k] = textureRatio[k].GetFloat();
        }
        

        meshConfig.maskTexture = mesh["maskTexture"].GetString();
        meshConfig.normalMap = mesh["normalMap"].GetString();
        meshConfig.enableShadow = mesh["enableShadow"].GetBool();
        meshConfig.isDynamicLOD = mesh["isDynamicLOD"].GetBool();

        if (mesh.HasMember("UV_Offset"))
        {
			const rapidjson::Value& UV_Offset = mesh["UV_Offset"];
			meshConfig.UV_Offset = glm::vec2(UV_Offset[0].GetFloat(), UV_Offset[1].GetFloat());
		}

        if (mesh.HasMember("UV_Tiling"))
        {
            const rapidjson::Value& UV_Tiling = mesh["UV_Tiling"];
            meshConfig.UV_Tiling = glm::vec2(UV_Tiling[0].GetFloat(), UV_Tiling[1].GetFloat());
        }

        if (mesh.HasMember("transperancy"))
        {
			meshConfig.transperancy = mesh["transperancy"].GetFloat();
		}

        gameObjectConfig.mesh = meshConfig;
    }

    if (gameObject.HasMember("light"))
    {
        const rapidjson::Value& light = gameObject["light"];
        LightConfig lightConfig = LightConfig();
        lightConfig.index = light["index"].GetInt();

        const rapidjson::Value& position = light["position"];
        lightConfig.position = glm::vec3(position[0].GetFloat(), position[1].GetFloat(), position[2].GetFloat());

        const rapidjson::Value& diffuse = light["diffuse"];
        lightConfig.diffuse = glm::vec3(diffuse[0].GetFloat(), diffuse[1].GetFloat(), diffuse[2].GetFloat());

        const rapidjson::Value& specular = light["specular"];
        lightConfig.specular = glm::vec3(specular[0].GetFloat(), specular[1].GetFloat(), specular[2].GetFloat());

        const rapidjson::Value& atten = light["atten"];
        lightConfig.atten = glm::vec3(atten[0].GetFloat(), atten[1].GetFloat(), atten[2].GetFloat());

        const rapidjson::Value& direction = light["direction"];
        lightConfig.direction = glm::vec3(direction[0].GetFloat(), direction[1].GetFloat(), direction[2].GetFloat());

        const rapidjson::Value& param1 = light["param1"];
        lightConfig.param1 = glm::vec3(param1[0].GetFloat(), param1[1].GetFloat(), param1[2].GetFloat());

        const rapidjson::Value& param2 = light["param2"];
        lightConfig.param2 = glm::vec3(param2[0].GetFloat(), 0, 0);

        gameObjectConfig.light = lightConfig;
    }
    else
    {
        LightConfig lightConfig;
		lightConfig.index = -1;
		gameObjectConfig.light = lightConfig;
    }

    if (gameObject.HasMember("physXObj"))
    {
        const rapidjson::Value& physXObj = gameObject["physXObj"];
        PhysXConfig physXConfig;
        physXConfig.type = (ColliderType)physXObj["type"].GetInt();


        physXConfig.isDynamic = physXObj["isDynamic"].GetBool();
        physXConfig.isTrigger = physXObj["isTrigger"].GetBool();     

        if (physXObj.HasMember("halfExtents"))
        {
            const rapidjson::Value& halfExtents = physXObj["halfExtents"];
            glm::vec3 halfExtentsVec = glm::vec3(halfExtents[0].GetFloat(), halfExtents[1].GetFloat(), halfExtents[2].GetFloat());
            physXConfig.halfExtents = halfExtentsVec;
            //p->UpdateBoxDimensions(halfExtentsVec);
            //p->halfExtents = halfExtentsVec;
        }
        else if (physXObj.HasMember("radius"))
        {
            physXConfig.radius = physXObj["radius"].GetFloat();
            // p->UpdateSphereDimensions(physXObj["radius"].GetFloat());
            //p->radius = physXObj["radius"].GetFloat();
        }


        gameObjectConfig.physXObj = physXConfig;
    }
    else
    {
        PhysXConfig physXConfig;
		physXConfig.type = NONE;
		gameObjectConfig.physXObj = physXConfig;
    }

    if (gameObject.HasMember("characterController"))
    {
        const rapidjson::Value& characterController = gameObject["characterController"];
        CharacterControllerConfig characterControllerConfig;
        characterControllerConfig.height = characterController["height"].GetFloat();
        characterControllerConfig.radius = characterController["radius"].GetFloat();

        const rapidjson::Value& position = characterController["position"];
        characterControllerConfig.position = glm::vec3(position[0].GetFloat(), position[1].GetFloat(), position[2].GetFloat());


        if (characterController.HasMember("stepOffset"))
        {
            characterControllerConfig.stepOffset= characterController["stepOffset"].GetFloat();
        }

        if (characterController.HasMember("slopeLimit"))
        {
			characterControllerConfig.slopeLimit = characterController["slopeLimit"].GetFloat();
		}

        gameObjectConfig.characterController = characterControllerConfig;
    }
    else
    {
		CharacterControllerConfig characterControllerConfig;
        characterControllerConfig.height = 0;
        characterControllerConfig.radius = 0;
    }

    if (gameObject.HasMember("children"))
    {
        rapidjson::Value& children = gameObject["children"];
        for (rapidjson::SizeType i = 0; i < children.Size(); i++)
        {
            rapidjson::Value& childValue = children[i];
            GameObjectConfig childConfig;
            GetLoadGameObjectConfig(childValue, childConfig);
            gameObjectConfig.children.push_back(childConfig);
        }
    }
}

GameObject* SceneSaver::LoadGameObject(GameObjectConfig& gameObjectConfig, Scene* scene)
{
    GameObject* go = scene->NewGameObject(gameObjectConfig.name);

    go->LoadAsync = gameObjectConfig.loadAsync;

    TransformConfig transformConfig = gameObjectConfig.transform;   

    go->AddComponent<TransformComponent>();
    TransformComponent* t = go->GetComponent<TransformComponent>();
    t->drawPosition = transformConfig.position;
    t->eulerRotation = transformConfig.rotation;
    t->drawScale = transformConfig.scale;

    if (!gameObjectConfig.mesh.fileName.empty())
    {
        MeshConfig meshConfig = gameObjectConfig.mesh;
        cMesh* m = &go->AddComponent<cMesh>(meshConfig.fileName, meshConfig.friendlyName);
        m->bIsWireframe = meshConfig.bIsWireframe;
        m->bDoNotLight = meshConfig.bDoNotLight;
        m->bIsVisible = meshConfig.bIsVisible;
        m->bUseDebugColours = meshConfig.bUseDebugColours;
        m->wholeObjectDebugColourRGBA = meshConfig.wholeObjectDebugColourRGBA;
        m->color = meshConfig.color;
        m->isSkyBox = meshConfig.isSkyBox;
        m->hasVertexColors = meshConfig.hasVertexColors;
        m->isReflective = meshConfig.isReflective;
        m->useBone = meshConfig.useBone;
        m->texture[0] = meshConfig.texture[0];
        m->texture[1] = meshConfig.texture[1];
        m->texture[2] = meshConfig.texture[2];
        m->texture[3] = meshConfig.texture[3];
        m->textureRatio[0] = meshConfig.textureRatio[0];
        m->textureRatio[1] = meshConfig.textureRatio[1];
        m->textureRatio[2] = meshConfig.textureRatio[2];
        m->textureRatio[3] = meshConfig.textureRatio[3];
        m->maskTexture = meshConfig.maskTexture;
        m->normalMap = meshConfig.normalMap;
        m->enableShadow = meshConfig.enableShadow;
        m->dynamicLOD = meshConfig.isDynamicLOD;
        m->UV_Offset = meshConfig.UV_Offset;
        m->UV_Tiling = meshConfig.UV_Tiling;
        m->transperancy = meshConfig.transperancy;
        m->transform = *t;
    }

    if (gameObjectConfig.light.index >= 0)
    {
        LightConfig lightConfig = gameObjectConfig.light;

        cLight* l = &go->AddComponent<cLight>();
        l->index = lightConfig.index;
        l->position = glm::vec4(lightConfig.position, 1.0);
        l->diffuse = glm::vec4(lightConfig.diffuse, 1.0);
        l->specular = glm::vec4(lightConfig.specular, 1.0);
        l->atten = glm::vec4(lightConfig.atten, 1.0);
        l->direction = glm::vec4(lightConfig.direction, 1.0);
        l->param1 = glm::vec4(lightConfig.param1, 1.0);
        l->param2 = glm::vec4(lightConfig.param2, 1.0);
        l->transform = t;
    }

    if (gameObjectConfig.physXObj.type != NONE)
    {
		PhysXConfig physXConfig = gameObjectConfig.physXObj;


		PhysXBody* p = &go->AddComponent<PhysXBody>(t);
        p->mesh = go->GetComponent<cMesh>();
		p->SetBody(physXConfig.isDynamic);
        p->SetShape(physXConfig.type);
        
        if (physXConfig.halfExtents.x > 0)
        {		
			p->UpdateBoxDimensions(physXConfig.halfExtents);
            p->halfExtents = physXConfig.halfExtents;
        }
		else if (physXConfig.radius > 0)
		{
			p->UpdateSphereDimensions(physXConfig.radius);
			p->radius = physXConfig.radius;
		}

        p->isTrigger = physXConfig.isTrigger;
        p->SetTrigger();
        p->GameObject = go;
	}

    if (gameObjectConfig.characterController.height > 0)
    {
		CharacterControllerConfig characterControllerConfig = gameObjectConfig.characterController;

		CharacterController* c = &go->AddComponent<CharacterController>(t);
		c->height = characterControllerConfig.height;
		c->radius = characterControllerConfig.radius;
		c->position = characterControllerConfig.position;
        c->stepOffset = characterControllerConfig.stepOffset;
        c->slopeLimit = characterControllerConfig.slopeLimit;
        c->controller->setHeight(c->height);
        c->controller->setRadius(c->radius);
        c->controller->setPosition(PxExtendedVec3(c->position.x, c->position.y, c->position.z));
        c->controller->setStepOffset(c->stepOffset);
        c->controller->setSlopeLimit(c->slopeLimit);
    }

   
    if (gameObjectConfig.children.size() > 0)
    {
        for (int i = 0; i < gameObjectConfig.children.size(); i++)
        {
            GameObjectConfig childConfig = gameObjectConfig.children[i];
            GameObject* child = LoadGameObject(childConfig, scene);
            go->m_Children.push_back(child);
		}
	}

    return go;
}

GameObject* SceneSaver::LoadGameObjectAsync(cMesh* loadedMesh, GameObjectConfig config, Scene* scene)
{
    
    GameObject* go = scene->NewGameObject(config.name);

    go->LoadAsync = config.loadAsync;

    TransformComponent* t = &go->AddComponent<TransformComponent>();

    t->drawPosition = config.transform.position;
    t->eulerRotation = config.transform.rotation;
    t->drawScale = config.transform.scale;


    cMesh* mesh = &go->AddComponent<cMesh>(loadedMesh);
    mesh->meshName = config.mesh.fileName;
    mesh->friendlyName = config.mesh.friendlyName;
    mesh->bIsWireframe = config.mesh.bIsWireframe;
    mesh->bDoNotLight = config.mesh.bDoNotLight;
    mesh->bIsVisible = config.mesh.bIsVisible;
    mesh->bUseDebugColours = config.mesh.bUseDebugColours;
    mesh->wholeObjectDebugColourRGBA = config.mesh.wholeObjectDebugColourRGBA;
    mesh->color = config.mesh.color;
    mesh->isSkyBox = config.mesh.isSkyBox;
    mesh->hasVertexColors = config.mesh.hasVertexColors;
    mesh->isReflective = config.mesh.isReflective;
    mesh->useBone = config.mesh.useBone;
    for (size_t i = 0; i < 4; i++)
    {
        mesh->texture[i] = config.mesh.texture[i];
        mesh->textureRatio[i] = config.mesh.textureRatio[i];
    }

    mesh->maskTexture = config.mesh.maskTexture;
    mesh->normalMap = config.mesh.normalMap;
    mesh->enableShadow = config.mesh.enableShadow;
    mesh->dynamicLOD = config.mesh.isDynamicLOD;

    if (config.light.index >= 0)
    {
        LightConfig lightConfig = config.light;

        cLight* l = &go->AddComponent<cLight>();
        l->index = lightConfig.index;
        l->position = glm::vec4(lightConfig.position, 1.0);
        l->diffuse = glm::vec4(lightConfig.diffuse, 1.0);
        l->specular = glm::vec4(lightConfig.specular, 1.0);
        l->atten = glm::vec4(lightConfig.atten, 1.0);
        l->direction = glm::vec4(lightConfig.direction, 1.0);
        l->param1 = glm::vec4(lightConfig.param1, 1.0);
        l->param2 = glm::vec4(lightConfig.param2, 1.0);
    }

    if (config.physXObj.type != NONE)
    {
        PhysXConfig physXConfig = config.physXObj;


        PhysXBody* p = &go->AddComponent<PhysXBody>(t);
        p->mesh = go->GetComponent<cMesh>();
        p->SetBody(physXConfig.isDynamic);
        p->SetShape(physXConfig.type);

        if (physXConfig.halfExtents.x > 0)
        {
            p->UpdateBoxDimensions(physXConfig.halfExtents);
            p->halfExtents = physXConfig.halfExtents;
        }
        else if (physXConfig.radius > 0)
        {
            p->UpdateSphereDimensions(physXConfig.radius);
            p->radius = physXConfig.radius;
        }

        p->isTrigger = physXConfig.isTrigger;
        p->SetTrigger();
    }

    if (config.characterController.height > 0)
    {
        CharacterControllerConfig characterControllerConfig = config.characterController;

        CharacterController* c = &go->AddComponent<CharacterController>(t);
        c->height = characterControllerConfig.height;
        c->radius = characterControllerConfig.radius;
        c->position = characterControllerConfig.position;
        c->controller->setHeight(c->height);
        c->controller->setRadius(c->radius);
    }


    return go;
}





