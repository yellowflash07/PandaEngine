
#include "Engine.h"
#include "Random.h"
#include <iostream>
#include "AnimationSystem.h"
#include <chrono>
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
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

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
    
    cMesh* skyBoxMesh = engine.LoadMesh("Sphere_1_unit_Radius_UV.ply", "skybox");
    skyBoxMesh->isSkyBox = true;
    skyBoxMesh->setUniformDrawScale(5000.0f);

    float currTime = 0;
    float myTime = 0;

    int frameNumber = 0;

    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

    while (!glfwWindowShouldClose(engine.window))
    {
        engine.Update();  
    }

    engine.ShutDown();
}
