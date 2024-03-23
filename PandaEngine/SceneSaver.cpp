#include "SceneSaver.h"
#include <fstream>
#include <sstream>

SceneSaver::SceneSaver()
{
    jsonReader = new JsonReader();
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


void SceneSaver::SaveScene(Scene* scene)
{

    SceneConfig sceneConfig;

    sceneConfig.sceneName = scene->name;

    for (size_t i = 0; i < scene->GetGameObjects().size(); i++)
    {
        GameObjectConfig gameObjectConfig;
        GameObject* go = scene->GetGameObjects()[i];    

        gameObjectConfig.name = go->m_Name;

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
			gameObjectConfig.mesh = meshConfig;
        }

        cLight* light = go->GetComponent<cLight>();
        if (light != nullptr)
        {
			LightConfig lightConfig;
			lightConfig.index = light->index;
			lightConfig.position = light->position;
			lightConfig.diffuse = light->diffuse;
			lightConfig.specular = light->specular;
			lightConfig.atten = light->atten;
			lightConfig.direction = light->direction;
			lightConfig.param1 = light->param1;
			lightConfig.param2 = light->param2;

			gameObjectConfig.light = lightConfig;
		}

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

        rapidjson::Value name;
        name.SetString(sceneConfig.gameObjects[i].name.c_str(), sceneConfig.gameObjects[i].name.size(), document.GetAllocator());

        gameObject.AddMember("name", name, document.GetAllocator());

        rapidjson::Value transform(rapidjson::kObjectType);
        rapidjson::Value position(rapidjson::kArrayType);
        position.PushBack(sceneConfig.gameObjects[i].transform.position.x, document.GetAllocator());
        position.PushBack(sceneConfig.gameObjects[i].transform.position.y, document.GetAllocator());
        position.PushBack(sceneConfig.gameObjects[i].transform.position.z, document.GetAllocator());
        transform.AddMember("position", position, document.GetAllocator());

        rapidjson::Value rotation(rapidjson::kArrayType);
        rotation.PushBack(sceneConfig.gameObjects[i].transform.rotation.x, document.GetAllocator());
        rotation.PushBack(sceneConfig.gameObjects[i].transform.rotation.y, document.GetAllocator());
        rotation.PushBack(sceneConfig.gameObjects[i].transform.rotation.z, document.GetAllocator());
        transform.AddMember("rotation", rotation, document.GetAllocator());

        rapidjson::Value scale(rapidjson::kArrayType);
        scale.PushBack(sceneConfig.gameObjects[i].transform.scale.x, document.GetAllocator());
        scale.PushBack(sceneConfig.gameObjects[i].transform.scale.y, document.GetAllocator());
        scale.PushBack(sceneConfig.gameObjects[i].transform.scale.z, document.GetAllocator());

        transform.AddMember("scale", scale, document.GetAllocator());
        gameObject.AddMember("transform", transform, document.GetAllocator());


        if (!sceneConfig.gameObjects[i].mesh.fileName.empty())
        {
            rapidjson::Value mesh(rapidjson::kObjectType);
            rapidjson::Value fileName;
            fileName.SetString(sceneConfig.gameObjects[i].mesh.fileName.c_str(), sceneConfig.gameObjects[i].mesh.fileName.size(), document.GetAllocator());
            mesh.AddMember("fileName", fileName, document.GetAllocator());

            rapidjson::Value friendlyName;
            friendlyName.SetString(sceneConfig.gameObjects[i].mesh.friendlyName.c_str(), sceneConfig.gameObjects[i].mesh.friendlyName.size(), document.GetAllocator());
            mesh.AddMember("friendlyName", friendlyName, document.GetAllocator());
            mesh.AddMember("bIsWireframe", sceneConfig.gameObjects[i].mesh.bIsWireframe, document.GetAllocator());
            mesh.AddMember("bDoNotLight", sceneConfig.gameObjects[i].mesh.bDoNotLight, document.GetAllocator());
            mesh.AddMember("bIsVisible", sceneConfig.gameObjects[i].mesh.bIsVisible, document.GetAllocator());
            mesh.AddMember("bUseDebugColours", sceneConfig.gameObjects[i].mesh.bUseDebugColours, document.GetAllocator());

            rapidjson::Value wholeObjectDebugColourRGBA(rapidjson::kArrayType);
            wholeObjectDebugColourRGBA.PushBack(sceneConfig.gameObjects[i].mesh.wholeObjectDebugColourRGBA.x, document.GetAllocator());
            wholeObjectDebugColourRGBA.PushBack(sceneConfig.gameObjects[i].mesh.wholeObjectDebugColourRGBA.y, document.GetAllocator());
            wholeObjectDebugColourRGBA.PushBack(sceneConfig.gameObjects[i].mesh.wholeObjectDebugColourRGBA.z, document.GetAllocator());
            wholeObjectDebugColourRGBA.PushBack(sceneConfig.gameObjects[i].mesh.wholeObjectDebugColourRGBA.w, document.GetAllocator());
            mesh.AddMember("wholeObjectDebugColourRGBA", wholeObjectDebugColourRGBA, document.GetAllocator());

            rapidjson::Value color(rapidjson::kArrayType);
            color.PushBack(sceneConfig.gameObjects[i].mesh.color.x, document.GetAllocator());
            color.PushBack(sceneConfig.gameObjects[i].mesh.color.y, document.GetAllocator());
            color.PushBack(sceneConfig.gameObjects[i].mesh.color.z, document.GetAllocator());
            color.PushBack(sceneConfig.gameObjects[i].mesh.color.w, document.GetAllocator());
            mesh.AddMember("color", color, document.GetAllocator());

            mesh.AddMember("isSkyBox", sceneConfig.gameObjects[i].mesh.isSkyBox, document.GetAllocator());
            mesh.AddMember("hasVertexColors", sceneConfig.gameObjects[i].mesh.hasVertexColors, document.GetAllocator());
            mesh.AddMember("isReflective", sceneConfig.gameObjects[i].mesh.isReflective, document.GetAllocator());
            mesh.AddMember("useBone", sceneConfig.gameObjects[i].mesh.useBone, document.GetAllocator());

            rapidjson::Value texture(rapidjson::kArrayType);
            for (size_t k = 0; k < 4; k++)
            {
                rapidjson::Value textureName;
                textureName.SetString(sceneConfig.gameObjects[i].mesh.texture[k].c_str(), sceneConfig.gameObjects[i].mesh.texture[k].size(), document.GetAllocator());
                texture.PushBack(textureName, document.GetAllocator());
            }

            mesh.AddMember("texture", texture, document.GetAllocator());

            rapidjson::Value textureRatio(rapidjson::kArrayType);
            for (size_t k = 0; k < 4; k++)
            {
                textureRatio.PushBack(sceneConfig.gameObjects[i].mesh.textureRatio[k], document.GetAllocator());
            }

            mesh.AddMember("textureRatio", textureRatio, document.GetAllocator());

            rapidjson::Value maskTexture;
            maskTexture.SetString(sceneConfig.gameObjects[i].mesh.maskTexture.c_str(), sceneConfig.gameObjects[i].mesh.maskTexture.size(), document.GetAllocator());
            mesh.AddMember("maskTexture", maskTexture, document.GetAllocator());

            gameObject.AddMember("mesh", mesh, document.GetAllocator());
        }
        

        if (sceneConfig.gameObjects[i].light.index >= 0)
        {
            rapidjson::Value light(rapidjson::kObjectType);
			light.AddMember("index", sceneConfig.gameObjects[i].light.index, document.GetAllocator());

			rapidjson::Value position(rapidjson::kArrayType);
			position.PushBack(sceneConfig.gameObjects[i].light.position.x, document.GetAllocator());
			position.PushBack(sceneConfig.gameObjects[i].light.position.y, document.GetAllocator());
			position.PushBack(sceneConfig.gameObjects[i].light.position.z, document.GetAllocator());
			light.AddMember("position", position, document.GetAllocator());

			rapidjson::Value diffuse(rapidjson::kArrayType);
			diffuse.PushBack(sceneConfig.gameObjects[i].light.diffuse.x, document.GetAllocator());
			diffuse.PushBack(sceneConfig.gameObjects[i].light.diffuse.y, document.GetAllocator());
			diffuse.PushBack(sceneConfig.gameObjects[i].light.diffuse.z, document.GetAllocator());
			light.AddMember("diffuse", diffuse, document.GetAllocator());

			rapidjson::Value specular(rapidjson::kArrayType);
			specular.PushBack(sceneConfig.gameObjects[i].light.specular.x, document.GetAllocator());
			specular.PushBack(sceneConfig.gameObjects[i].light.specular.y, document.GetAllocator());
			specular.PushBack(sceneConfig.gameObjects[i].light.specular.z, document.GetAllocator());
			light.AddMember("specular", specular, document.GetAllocator());

			rapidjson::Value atten(rapidjson::kArrayType);
			atten.PushBack(sceneConfig.gameObjects[i].light.atten.x, document.GetAllocator());
			atten.PushBack(sceneConfig.gameObjects[i].light.atten.y, document.GetAllocator());
			atten.PushBack(sceneConfig.gameObjects[i].light.atten.z, document.GetAllocator());
			light.AddMember("atten", atten, document.GetAllocator());

			rapidjson::Value direction(rapidjson::kArrayType);
			direction.PushBack(sceneConfig.gameObjects[i].light.direction.x, document.GetAllocator());
			direction.PushBack(sceneConfig.gameObjects[i].light.direction.y, document.GetAllocator());
			direction.PushBack(sceneConfig.gameObjects[i].light.direction.z, document.GetAllocator());
			light.AddMember("direction", direction, document.GetAllocator());

			rapidjson::Value param1(rapidjson::kArrayType);
			param1.PushBack(sceneConfig.gameObjects[i].light.param1.x, document.GetAllocator());
			param1.PushBack(sceneConfig.gameObjects[i].light.param1.y, document.GetAllocator());
           	param1.PushBack(sceneConfig.gameObjects[i].light.param1.z, document.GetAllocator());
            light.AddMember("param1", param1, document.GetAllocator());

            rapidjson::Value param2(rapidjson::kArrayType);
            param2.PushBack(sceneConfig.gameObjects[i].light.param2.x, document.GetAllocator());
            light.AddMember("param2", param2, document.GetAllocator());

            gameObject.AddMember("light", light, document.GetAllocator());

        }

        gameObjects.PushBack(gameObject, document.GetAllocator());
    }

    document.AddMember("gameObjects", gameObjects, document.GetAllocator());

    jsonReader->WriteJsonFile("scene.json", document);

}

Scene* SceneSaver::LoadScene(std::string sceneFile)
{
    rapidjson::Document document;
    if (!jsonReader->LoadJsonFile(sceneFile.c_str(), document))
    {
        std::cerr << "Failed to load scene file\n";
		return nullptr;
	}

   
	std::string sceneName = document["sceneName"].GetString();
    Scene* scene = new Scene(sceneName);

	const rapidjson::Value& gameObjects = document["gameObjects"];
    for (rapidjson::SizeType i = 0; i < gameObjects.Size(); i++)
    {
		GameObjectConfig gameObjectConfig;
		const rapidjson::Value& gameObject = gameObjects[i];

		gameObjectConfig.name = gameObject["name"].GetString();


        TransformConfig transformConfig = TransformConfig();
        const rapidjson::Value& transform = gameObject["transform"];
        transformConfig.position = glm::vec3(transform["position"][0].GetFloat(), transform["position"][1].GetFloat(), transform["position"][2].GetFloat());
        transformConfig.rotation = glm::vec3(transform["rotation"][0].GetFloat(), transform["rotation"][1].GetFloat(), transform["rotation"][2].GetFloat());
        transformConfig.scale = glm::vec3(transform["scale"][0].GetFloat(), transform["scale"][1].GetFloat(), transform["scale"][2].GetFloat());

        GameObject* go = scene->CreateGameObject(gameObjectConfig.name);

        TransformComponent* t = go->GetComponent<TransformComponent>();
        t->drawPosition = transformConfig.position;
        t->eulerRotation = transformConfig.rotation;
        t->drawScale = transformConfig.scale;

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
			lightConfig.param2 = glm::vec3(param2[0].GetFloat(),0, 0);

			cLight* l = &go->AddComponent<cLight>();
            l->index = lightConfig.index;
			l->position = glm::vec4(lightConfig.position,1.0);
			l->diffuse = glm::vec4(lightConfig.diffuse, 1.0);
            l->specular = glm::vec4(lightConfig.specular, 1.0);
            l->atten = glm::vec4(lightConfig.atten, 1.0);
            l->direction = glm::vec4(lightConfig.direction, 1.0);
            l->param1 = glm::vec4(lightConfig.param1, 1.0);
            l->param2 = glm::vec4(lightConfig.param2, 1.0);
        }

    }

    return scene;

}





