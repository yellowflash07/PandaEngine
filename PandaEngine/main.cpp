
#include "Engine.h"
#include "Random.h"
#include <iostream>
#include "AnimationSystem.h"
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
    /*"right.jpg",
        "left.jpg",
        "top.jpg",
        "bottom.jpg",
        "front.jpg",
        "back.jpg"*/
    engine.meshManager->LoadTexture("PaletteV1.bmp");
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

    cMesh* mesh = engine.meshManager->FindMeshByFriendlyName("rigged");
    mesh->useBone = true;

    AnimationSystem* animationSystem = new AnimationSystem();
    animationSystem->meshManager = engine.meshManager;
    Animation* animation = new Animation("test", mesh);
    animationSystem->AddAnimation(animation);

    float currTime = 0;
    float myTime = 0;

    int frameNumber = 0;

    while (!glfwWindowShouldClose(engine.window))
    {
        engine.Update();  


        if (keyHit == GLFW_KEY_SPACE)
        {
            animationSystem->debug += 10.0f;
            //keyHit = 0;
        }

        frameNumber+=10.0f;
        if (frameNumber > 3433) frameNumber = 0;
        float val = (float)frameNumber;

        animationSystem->Update(val);
    }

    engine.ShutDown();
}
