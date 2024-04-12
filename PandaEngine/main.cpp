
#include "Engine.h"
#include "Random.h"
#include <iostream>
#include "AnimationSystem.h"
//#include <px>
#include "PhysXManager.h"
#include "PhysXBody.h"
#include "Raycast.h"
#include <glm/gtx/string_cast.hpp>
#include "Debug.h"
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

    camera->SetPosition(glm::vec3(11.0f,37.0f, 401.0f));

    engine.LoadSave(); 
    float currTime = 0;
    float myTime = 0;

    camera->speed = 150.0f;

    Scene* scene = engine.GetCurrentScene();

    float blendWeight = 0;
    float tessLevelOuter = 1.0f;
    float tessLevelInner = 1.0f;
    GLint shaderProgramID = cShaderManager::getInstance()->getIDFromFriendlyName("shader01");
    while (!glfwWindowShouldClose(engine.window))
    {

        engine.BeginRender();

        engine.Update();   

        ImGui::Begin("Debug");

      //  uniform float tessLevelOuter;
      //  uniform float tessLevelInner;
        if (ImGui::DragFloat("Tess Outer", &tessLevelOuter, 0.1f, -10.0f, 64.0f))
        {
            GLint tessLevelOuter_UL = glGetUniformLocation(shaderProgramID, "tessLevelOuter");
            glUniform1f(tessLevelOuter_UL, tessLevelOuter);
		}
        if (ImGui::DragFloat("Tess Inner", &tessLevelInner, 0.1f, -10.0f, 64.0f))
        {
            GLint tessLevelInner_UL = glGetUniformLocation(shaderProgramID, "tessLevelInner");
            glUniform1f(tessLevelInner_UL, tessLevelInner);
        }

        ImGui::End();
        engine.EndRender();   
    }

    engine.ShutDown();
}
