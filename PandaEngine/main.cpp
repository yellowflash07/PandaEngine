
#include "Engine.h"
#include "Random.h"
#include <iostream>
#include "AnimationSystem.h"
extern Camera* camera;
int keyHit = 0;

#include "Scene.h"

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

    bool loaded = engine.meshManager->LoadCubeMap("space",
                                    "CubeMaps/TropicalSunnyDayLeft2048.bmp",
                                    "CubeMaps/TropicalSunnyDayRight2048.bmp",
                                    "CubeMaps/TropicalSunnyDayDown2048.bmp",
                                    "CubeMaps/TropicalSunnyDayUp2048.bmp",
                                    "CubeMaps/TropicalSunnyDayFront2048.bmp",
                                    "CubeMaps/TropicalSunnyDayBack2048.bmp",
                                    true);

    camera->SetPosition(glm::vec3(35.0f, 150.0f, 500.0f));

    engine.LoadSave();
   

    Scene* scene = new Scene("Test.pda");

    engine.AddScene(scene);

    GameObject* skyBox = scene->CreateGameObject("SkyBox");
    cMesh& skyBoxMesh = skyBox->AddComponent<cMesh>("Sphere_1_unit_Radius_UV.ply", "SkyBox");
    skyBoxMesh.isSkyBox = true;
    skyBox->GetComponent<TransformComponent>()->setUniformDrawScale(5000.0f);
    

    GameObject* go = scene->CreateGameObject("Sphere");
    cMesh &mesh = go->AddComponent<cMesh>("Sphere_1_unit_Radius_UV.ply", "FriendlyName");
   // go->AddComponent<AnimationSystem>();

    GameObject* lightGo = scene->CreateGameObject("Light1");
    lightGo->AddComponent<cLight>();

   // go->AddComponent<cLight>();
    //go->AddComponent<cLight>();

    float currTime = 0;
    float myTime = 0;

    while (!glfwWindowShouldClose(engine.window))
    {
        engine.Update();  
    }

    engine.ShutDown();
}
