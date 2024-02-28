
#include "Engine.h"
#include "Random.h"
#include <iostream>
#include "SoftBodyManager.h"
#include "RenderTexture.h"

extern Camera* camera;
int keyHit = 0;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS)
    {
        keyHit = key;
        return;
    }
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    camera->ProcessMouseMovement(xpos, ypos);
}

int main(void)
{
    Engine engine;
    if (!engine.Initialize())
    {
        return 1;
    }

    engine.meshManager->LoadTexture("PaletteV1.bmp");
    bool loaded = engine.meshManager->LoadCubeMap("space",
                                    "CubeMaps/TropicalSunnyDayLeft2048.bmp",
                                    "CubeMaps/TropicalSunnyDayRight2048.bmp",
                                    "CubeMaps/TropicalSunnyDayUp2048.bmp",
                                    "CubeMaps/TropicalSunnyDayDown2048.bmp",
                                    "CubeMaps/TropicalSunnyDayFront2048.bmp",
                                    "CubeMaps/TropicalSunnyDayBack2048.bmp",
                                    true);

    camera->SetPosition(glm::vec3(35.0f, 150.0f, 500.0f));

    engine.LoadSave();
    
    cMesh* skyBoxMesh = engine.LoadMesh("Sphere_1_unit_Radius_UV.ply", "skybox");
    skyBoxMesh->isSkyBox = true;
    skyBoxMesh->setUniformDrawScale(5000.0f);  


    SoftBodyManager* softBodyManager = new SoftBodyManager(engine.meshManager);

    SoftBody* softBody = softBodyManager->CreateSoftBody("GSoftBody.ply");
    softBody->acceleration = glm::vec3(0.0f, -9.8f/2000.0f, 0.0f);

    std::vector<cMesh*> vecMeshes;

    PhysicsBody* pBody = new PhysicsBody();
    pBody->mesh = engine.meshManager->FindMeshByFriendlyName("Sphere_1_unit_Radius_UV.ply");
    pBody->shapeType = PhysicsShapes::SPHERE;
    pBody->setShape(new PhysicsShapes::sSphere(pBody->mesh->drawScale.x));  
    pBody->inverseMass = 0.0f;

   // softBody->AddCollisionSphere(pBody);

    engine.meshManager->RemoveMesh("TieFighter");

    cMesh* grid = engine.meshManager->FindMeshByFriendlyName("GSoftBody.ply");
    grid->bDoNotLight = false;

    for (size_t i = 0; i < softBody->vec_pParticles.size(); i++)
    {
      //  cMesh* sphere = engine.LoadMesh("Sphere_1_unit_Radius_UV.ply", "sphere" + std::to_string(i));
     //   sphere->drawPosition = softBody->vec_pParticles[i]->position;
     //   vecMeshes.push_back(sphere);
    }

    float currTime = 0;
    float myTime = 0;

    while (!glfwWindowShouldClose(engine.window))
    {
        engine.Update();   
        softBodyManager->Update(engine.deltaTime);

       /* for (size_t i = 0; i < softBody->vec_pParticles.size(); i++)
        {
			vecMeshes[i]->drawPosition = softBody->vec_pParticles[i]->position;
		}*/
    }

    engine.ShutDown();
}
