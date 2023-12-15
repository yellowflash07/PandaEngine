
#include "Engine.h"
#include "Random.h"
#include <iostream>
#include <map>
#include "CommandFactory.h"
#include <Lua5.4.6/lua.hpp>
#include <sol/sol.hpp>


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

#pragma region Skybox
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

#pragma endregion

    sol::state lua;

 
	cMesh* sphere = engine.LoadMesh("Sphere_1_unit_Radius_UV.ply", "sphere");
    sphere->bDoNotLight = true;
    sphere->bUseDebugColours = true;
    sphere->bIsVisible = false;
    sphere->drawPosition = glm::vec3(0, 0, 100);

    cMesh* mesh = engine.LoadMesh("bathtub_xyz_n_rgba.ply", "mesh");
 
    mesh->bUseDebugColours = true;
    mesh->wholeObjectDebugColourRGBA = glm::vec4(1, 0, 0, 1);


 lua.new_usertype<MoveTo>("MoveTo",
        sol::constructors<MoveTo(), MoveTo(cMesh*, glm::vec3, float)>(),
        "SetParams", sol::overload(
            static_cast<void (MoveTo::*)(cMesh*, glm::vec3, float)>(&MoveTo::SetParams),
            static_cast<void (MoveTo::*)(cMesh*, cMesh*, float)>(&MoveTo::SetParams)
        ),
        "Execute", &MoveTo::Execute
    );

 
     CommandFactory commandFactory;
	MoveTo* moveTo = (MoveTo*)commandFactory.CreateCommand("MoveTo");
    moveTo->SetParams(mesh, sphere->drawPosition, 20.5f);


  engine.LoadDefaultLights();

    float currTime = 0;
    float myTime = 0;
    while (!glfwWindowShouldClose(engine.window))
    {
        engine.Update();
        skyBoxMesh->drawPosition = camera->cameraEye;
        currTime += engine.deltaTime;

      
        if (glfwGetKey(engine.window, GLFW_KEY_1) == GLFW_PRESS)
        {
            
            sol::function executeFunc = lua["MoveTo"]["Execute"];
            executeFunc(moveTo, engine.deltaTime);
            
        }
    }

    engine.ShutDown();
}
