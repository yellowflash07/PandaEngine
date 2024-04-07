
#include "Engine.h"
#include "Random.h"
#include <iostream>
#include "AnimationSystem.h"
//#include <px>
#include "PhysXManager.h"
#include "PhysXBody.h"
#include <glm/gtx/string_cast.hpp>
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

    camera->SetPosition(glm::vec3(11.0f,158.0f, 401.0f));

    engine.LoadSave(); 

    float currTime = 0;
    float myTime = 0;

    camera->speed = 150.0f;

    Scene* scene = engine.GetCurrentScene();

    //sphere
    GameObject* sphere = scene->GetGameObjectByName("Sphere");
    PhysXBody* sphereBody = sphere->GetComponent<PhysXBody>();

    sphereBody->onContactStart = [](glm::vec3 contactPoint) {
		std::cout << "Sphere contact at" << glm::to_string(contactPoint)<< std::endl;
	};

    sphereBody->onTriggerEnter = [](PhysXBody* other) {
		std::cout << "Sphere entered trigger" << std::endl;
	};

    sphereBody->onTriggerExit = [](PhysXBody* other) {
        std::cout << "Sphere exit trigger" << std::endl;
    };




    while (!glfwWindowShouldClose(engine.window))
    {

        engine.BeginRender();

        engine.Update();   

        engine.EndRender();   
    }

    engine.ShutDown();
}
