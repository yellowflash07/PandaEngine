
#include "Engine.h"
#include "Random.h"
#include <iostream>
#include "AnimationSystem.h"
//#include <px>
#include "PhysXManager.h"
#include "PhysXBody.h"
#include "Raycast.h"
#include <glm/gtx/string_cast.hpp>
extern Camera* camera;
int keyHit = 0;

#include "Scene.h"

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS)
    {
        keyHit = key;
    }
    if (action == GLFW_RELEASE)
    {
		keyHit = 0;
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

    camera->SetPosition(glm::vec3(11.0f,158.0f, 401.0f));

    engine.LoadSave(); 

    float currTime = 0;
    float myTime = 0;

    camera->speed = 150.0f;

    Scene* scene = engine.GetCurrentScene();

    //get the dancer object
    GameObject* dancer = scene->GetGameObjectByName("Dancer");
    cMesh* dancerMesh = dancer->GetComponent<cMesh>(); //get the mesh of the dancer
    //add animation component
    AnimationSystem* animation = &dancer->AddComponent<AnimationSystem>();
    animation->m_mesh = dancerMesh;
    //load animations
    animation->LoadAnimationFromFile("Silly Dancing.fbx");
    animation->LoadAnimationFromFile("Hip Hop Dancing.fbx");
    animation->LoadAnimationFromFile("Rumba Dancing.fbx");
    animation->LoadAnimationFromFile("Bboy Hip Hop Move.fbx");

    float blendWeight = 0;

    while (!glfwWindowShouldClose(engine.window))
    {

        engine.BeginRender();

        engine.Update(); 

        if (keyHit == GLFW_KEY_1)
        {
			animation->SetBlend(1, 0, blendWeight);
            blendWeight += 0.1f;
            printf("Blend Weight: %f\n", blendWeight);
		}
		

        engine.EndRender();   
    }

    engine.ShutDown();
}
