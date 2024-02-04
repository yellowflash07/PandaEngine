
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

    AnimationSystem* animSystem = new AnimationSystem();

    Animation* anim = new Animation();
    anim->name = "TieFighter";
    anim->mesh = mesh;
    anim->positionKeyFrames.push_back(PositionKeyFrame(glm::vec3(0.0f, 0.0f, 20.0f), 0.0, EasingType::SineEaseIn));
    anim->positionKeyFrames.push_back(PositionKeyFrame(glm::vec3(0.0f, 0.0f, -1000.0f), 2.0, EasingType::SineEaseIn));

    anim->rotationKeyFrames.push_back(RotationKeyFrame(glm::vec3(0.0f, 0.0f, 0.0f), 0.0, EasingType::SineEaseIn));
    anim->rotationKeyFrames.push_back(RotationKeyFrame(glm::vec3(0.0f, 0.0f, 1.6f), 2.0, EasingType::SineEaseIn));

    anim->scaleKeyFrames.push_back(ScaleKeyFrame(glm::vec3(1.0f, 1.0f, 1.0f), 0.0, EasingType::SineEaseIn));

    animSystem->AddAnimation(anim);

    glm::vec3 direction = glm::normalize(mesh->drawPosition - camera->cameraEye);
    camera->cameraTarget = direction;

    float currTime = 0;
    float myTime = 0;
    unsigned long long frameNumber = 0;
    bool togglePause = false;
    bool reverse = false;
    while (!glfwWindowShouldClose(engine.window))
    {
        engine.Update();      

        if (keyHit == GLFW_KEY_SPACE)
        {
            togglePause = !togglePause;
            keyHit = 0;
        }

        if (keyHit == GLFW_KEY_1)
        {
            animSystem->animationSpeed = 1.0f;
			keyHit = 0;
		}
        if (keyHit == GLFW_KEY_2)
        {
			animSystem->animationSpeed = 2.0f;
            keyHit = 0;
        }
        if (keyHit == GLFW_KEY_3)
		{
            animSystem->animationSpeed = 3.0f;
            keyHit = 0;
        }
        if (keyHit == GLFW_KEY_4)
        {
            animSystem->animationSpeed = 4.0f;
            keyHit = 0;
        }
        if (keyHit == GLFW_KEY_5)
        {
            animSystem->animationSpeed = 5.0f;
            keyHit = 0;
        }
        

        if (togglePause)
        {
			animSystem->Pause();
		}
        else
        {
			animSystem->Resume();         
             
            if (keyHit == GLFW_KEY_R)
            {
                reverse = !reverse;
                keyHit = 0;
            }

            if (reverse)
            {
				frameNumber--;
			}
            else
            {
				frameNumber++;
			}

            if (frameNumber > 1000) frameNumber = 0;
            float val = (float)frameNumber / 250.f;

            animSystem->Update(val);
		}
    }

    engine.ShutDown();
}
