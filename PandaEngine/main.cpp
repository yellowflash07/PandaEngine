
#include "Engine.h"
#include "Random.h"
#include <iostream>
#include <map>
#include "CommandFactory.h"

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

    cMesh* sphere = engine.LoadMesh("Sphere_1_unit_Radius_UV.ply", "sphere");
    sphere->bDoNotLight = true;
    sphere->bUseDebugColours = true;
    sphere->bIsVisible = false;
    sphere->drawPosition = glm::vec3(0, 0, 10);

    cMesh* mesh = engine.LoadMesh("bathtub_xyz_n_rgba.ply", "mesh");
   // mesh->bDoNotLight = true;
    mesh->bUseDebugColours = true;
    mesh->wholeObjectDebugColourRGBA = glm::vec4(1, 0, 0, 1);

     CommandFactory commandFactory;
    OrientTo* orientTo = (OrientTo*)commandFactory.CreateCommand("OrientTo");

    orientTo->SetParams(mesh, sphere->drawPosition,0.0f, 10.0f);

    MoveTo* moveTo = (MoveTo*)commandFactory.CreateCommand("MoveTo");
    moveTo->SetParams(mesh, sphere->drawPosition, 0.5f);

    FollowObject* followObject = (FollowObject*)commandFactory.CreateCommand("FollowObject");
    followObject->SetParams(mesh, sphere, glm::vec3(0, 10, 0), 0.5f, 1.0f);

    FollowCurve* followCurve = (FollowCurve*)commandFactory.CreateCommand("FollowCurve");
    std::vector<glm::vec3> curvePoints;
    curvePoints.push_back(glm::vec3(0, 0, 0));
    curvePoints.push_back(glm::vec3(10, 0, 30));
    curvePoints.push_back(glm::vec3(20, 0, 50));
    curvePoints.push_back(glm::vec3(48, 0, 73));
    curvePoints.push_back(glm::vec3(65, 0, 95));
    curvePoints.push_back(glm::vec3(90, 0, 110));
    curvePoints.push_back(glm::vec3(105, 0, 130));

    followCurve->SetParams(mesh, curvePoints, 0.5f, 1.0f);

    for (glm::vec3 curvPt : curvePoints)
    {
        cMesh* sphere = engine.LoadMesh("Sphere_1_unit_Radius_UV.ply", "sphere");
        sphere->bDoNotLight = true;
        sphere->bUseDebugColours = true;

        sphere->drawPosition = curvPt;
    }

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
            followCurve->Execute(engine.deltaTime);
        }
    }

    engine.ShutDown();
}
