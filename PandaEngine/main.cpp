
#include "Engine.h"
#include "Random.h"
#include <iostream>
#include <map>

extern Camera* camera;
int keyHit = 0;
int action = 0;
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS)
    {
        //   std::cout << "Hit" << key << std::endl;
        keyHit = key;
        action = action;
    }
}
float pitch = 0.0f;
float yaw = 0.0f;
float roll = 0.0f;

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    //camera->ProcessMouseMovement(xpos, ypos);
    static bool firstMouse = true;
    static float lastX = 0.0f;
    static float lastY = 0.0f;

    if (firstMouse)
    {
        lastX = (float)xpos;
        lastY = (float)ypos;
        firstMouse = false;
    }


    float xoffset = (float)xpos - lastX;
    float yoffset = lastY - (float)ypos;

    lastX = (float)xpos;
    lastY = (float)ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;
}

int main(void)
{
    Engine engine;
    if (!engine.Initialize())
    {
        return 1;
    }

  //  engine.meshManager->LoadTexture("Water_Texture_01.bmp");
  //  engine.meshManager->LoadTexture("TaylorSwift_Eras_Poster.bmp");
  //  engine.meshManager->LoadTexture("FAKE_Stencil_Texture_612x612.bmp");
    engine.meshManager->LoadTexture("PaletteV1.bmp");
    bool loaded = engine.meshManager->LoadCubeMap("space",
                                    "CubeMaps/TropicalSunnyDayLeft2048.bmp",
                                    "CubeMaps/TropicalSunnyDayRight2048.bmp",
                                    "CubeMaps/TropicalSunnyDayUp2048.bmp",
                                    "CubeMaps/TropicalSunnyDayDown2048.bmp",
                                    "CubeMaps/TropicalSunnyDayFront2048.bmp",
                                    "CubeMaps/TropicalSunnyDayBack2048.bmp",
                                    true);
    
    cMesh* skyBoxMesh = engine.LoadMesh("Sphere_1_unit_Radius_UV.ply", "skybox");
    skyBoxMesh->isSkyBox = true;
    skyBoxMesh->setUniformDrawScale(5000.0f);

    cMesh* bird = engine.LoadMesh("BirdOfPrey.ply", "bird");
    bird->drawPosition = glm::vec3(0.0f, 30.0f, -200.0f);
    bird->drawScale = glm::vec3(0.01f, 0.01f, 0.01f);   
    PhysicsBody* body = engine.AddPhysicsBody("bird");
    body->shapeType = PhysicsShapes::AABB;

    //not really using this AABB, this is simply to filter
    body->setShape(new PhysicsShapes::sAABB(glm::vec3(0), glm::vec3(0)));

    engine.physicsManager->GenerateAABBs(body, 5);    


    cMesh* city = engine.LoadMesh("cartoonCity_Showcase_rgba.ply", "city");
    city->transperancy = 1.0f;
    city->bDoNotLight = true;
    city->texture[0] = "PaletteV1.bmp";
    city->textureRatio[0] = 1.0f;
    city->drawPosition = glm::vec3(0.0f, 0.0f, 0.0f);
    city->setRotationFromEuler(glm::vec3(-1.6f, 0.0f, 0.0f));

    PhysicsBody* body1 = engine.AddPhysicsBody("city");
    body1->inverseMass = 0;
    body1->shapeType = PhysicsShapes::MESH_OF_TRIANGLES_INDIRECT;
    body1->setShape(new PhysicsShapes::sMeshOfTriangles_Indirect("city"));
    
    engine.physicsManager->GenerateAABBs(body1,10);

    body->inverseMass = 1.0f / 10.0f;
   // body->acceleration = glm::vec3(0.0f, -9.8f/10.0f, 0.0f);

    std::map<cMesh*, cAABB*> sphereAABBmeshes;
    std::map<cMesh*, cAABB*> meshes;

    for (size_t i = 0; i < body->aabbs.size(); i++)
    {
        cMesh* mesh = engine.LoadMesh("Cube_1x1x1_xyz_n_rgba.ply", "SPHEREAABB" + std::to_string(i));
        //mesh->color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
        mesh->drawScale = body->aabbs[i]->getExtentsXYZ();
        mesh->drawPosition = body->aabbs[i]->getCentreXYZ();
        mesh->bDoNotLight = true;
        mesh->bUseDebugColours = true;
        mesh->wholeObjectDebugColourRGBA = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
        mesh->drawPosition = body->aabbs[i]->getCentreXYZ();
        mesh->bIsWireframe = true;
        sphereAABBmeshes[mesh] = body->aabbs[i];
    }

    int count = 0;
    for (size_t i = 0; i < body1->aabbs.size(); i++)
    {

        cMesh* mesh = engine.LoadMesh("Cube_1x1x1_xyz_n_rgba.ply", "AABB" + std::to_string(i));
        //mesh->color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
        mesh->drawScale = body1->aabbs[i]->getExtentsXYZ();
        mesh->drawPosition = body1->aabbs[i]->getCentreXYZ();
        mesh->bIsVisible = true;
        mesh->bDoNotLight = true;
        mesh->bUseDebugColours = true;
        mesh->wholeObjectDebugColourRGBA = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
        mesh->bIsWireframe = true;
        meshes[mesh] = body1->aabbs[i];
    }

    std::map<cMesh*, cAABB*>::iterator it;
    engine.LoadDefaultLights();

    while (!glfwWindowShouldClose(engine.window))
    {
        engine.Update();
        skyBoxMesh->drawPosition = camera->cameraEye;

        glm::vec3 cameraPos = glm::vec3(bird->drawPosition.x, bird->drawPosition.y + 5.0f, bird->drawPosition.z -10.0f);
        camera->Follow(cameraPos, glm::normalize(bird->drawPosition - cameraPos));
        if (glfwGetKey(engine.window, GLFW_KEY_W) == GLFW_PRESS)
        {
			body->velocity.z += 0.1f;
		}
        if (glfwGetKey(engine.window, GLFW_KEY_S) == GLFW_PRESS)
        {
            body->velocity.z -= 0.1f;
        }
        if (glfwGetKey(engine.window, GLFW_KEY_A) == GLFW_PRESS)
        {
            body->velocity.x += 0.1f;
		}
        if (glfwGetKey(engine.window, GLFW_KEY_D) == GLFW_PRESS)
        {
            body->velocity.x -= 0.1f;
        }
        if (glfwGetKey(engine.window, GLFW_KEY_SPACE) == GLFW_PRESS)
        {
			body->velocity.y += 0.1f;
		}
        if (glfwGetKey(engine.window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        {
			body->velocity.y -= 0.1f;
		}

        for (it = sphereAABBmeshes.begin(); it != sphereAABBmeshes.end(); it++)
        {
            it->first->drawPosition = it->second->getCentreXYZ();
            for (size_t i = 0; i < body->aabbPairs.size(); i++)
            {
                if (body->aabbPairs[i].first == it->second || body->aabbPairs[i].second == it->second)
                {
                    it->first->wholeObjectDebugColourRGBA = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
                    continue;
				}
                it->first->wholeObjectDebugColourRGBA = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
            }
        }

        for (it = meshes.begin(); it != meshes.end(); it++)
        {
            it->first->drawPosition = it->second->getCentreXYZ();
            for (size_t i = 0; i < body1->aabbPairs.size(); i++)
            {
                if (body1->aabbPairs[i].first == it->second)
                {
                    it->first->wholeObjectDebugColourRGBA = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
                    continue;
                }
                it->first->wholeObjectDebugColourRGBA = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);

            }
        }

    }

    engine.ShutDown();
}
