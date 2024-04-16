
#include "Engine.h"
#include "Random.h"
#include <iostream>
#include <glm/gtx/string_cast.hpp>
#include "Debug.h"
extern Camera* camera;
int keyHit = 0;

#include "Scene.h"
#include "noiseGenerator.h"

std::map<int, bool> keys;

bool IsKeyPressed(int key)
{
    return keys[key];
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS)
    {
        keyHit = key;
        keys[key] = true;
    }
    if (action == GLFW_RELEASE)
    {
        keyHit = 0;
        keys[key] = false;
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

    camera->SetPosition(glm::vec3(4.0f, 128.0f,714.0f));

    engine.LoadSave();
    printf("DONE\n");
    float currTime = 0;
    float myTime = 0;

    camera->speed = 150.0f;

    Scene* scene = engine.GetCurrentScene();

    //left hand

    GameObject* go = scene->GetGameObjectByName("vampire");
    AnimationSystem* anim = &go->AddComponent<AnimationSystem>();
    cMesh* mesh = go->GetComponent<cMesh>();

    TransformComponent* sphere = scene->GetGameObjectByName("Sphere")->GetComponent<TransformComponent>();


    bool hasReversed = false;
    while (!glfwWindowShouldClose(engine.window))
    {
        engine.BeginRender();

        engine.Update();     

        anim->AttachObjectToBone("LeftHand", sphere);

        ImGui::Begin("Debug");
        ImGui::Text("FPS: %f", 1 / engine.deltaTime);
        ImGui::End();
        engine.EndRender();
    }

    engine.ShutDown();
}
