
#include "Engine.h"
#include "Random.h"
#include <iostream>
#include <map>
#include "Animation.h"
#include "AnimationSystem.h"


extern Camera* camera;
int keyHit = 0;
int action = 0;
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS)
    {
       // std::cout << "Hit" << key << std::endl;
        keyHit = key;
        action = action;
        return;
    }
}

float pitch = 0.0f;
float yaw = 0.0f;
float roll = 0.0f;

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    camera->ProcessMouseMovement(xpos, ypos);
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

    engine.meshManager->LoadTexture("PaletteV1.bmp");
    bool loaded = engine.meshManager->LoadCubeMap("space",
                                    "CubeMaps/TropicalSunnyDayLeft2048.bmp",
                                    "CubeMaps/TropicalSunnyDayRight2048.bmp",
                                    "CubeMaps/TropicalSunnyDayUp2048.bmp",
                                    "CubeMaps/TropicalSunnyDayDown2048.bmp",
                                    "CubeMaps/TropicalSunnyDayFront2048.bmp",
                                    "CubeMaps/TropicalSunnyDayBack2048.bmp",
                                    true);

    camera->SetPosition(glm::vec3(0.0f, 400.0f, 200.0f));
    
    cMesh* skyBoxMesh = engine.LoadMesh("Sphere_1_unit_Radius_UV.ply", "skybox");
    skyBoxMesh->isSkyBox = true;
    skyBoxMesh->setUniformDrawScale(5000.0f);

    cMesh* mesh = engine.LoadMesh("tie_fighter.ply", "tie_fighter");
    mesh->bUseDebugColours = true;


    float currTime = 0;
    float myTime = 0;

    while (!glfwWindowShouldClose(engine.window))
    {
        engine.Update();      
    }

    engine.ShutDown();
}
