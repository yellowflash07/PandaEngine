
#include "Engine.h"
#include "Random.h"
#include <iostream>
#include "SoftBodyManager.h"
#include "Animation.h"
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

    engine.meshManager->LoadTexture("PaletteV1.bmp");
    bool loaded = engine.meshManager->LoadCubeMap("space",
                                    "CubeMaps/TropicalSunnyDayLeft2048.bmp",
                                    "CubeMaps/TropicalSunnyDayRight2048.bmp",
                                    "CubeMaps/TropicalSunnyDayUp2048.bmp",
                                    "CubeMaps/TropicalSunnyDayDown2048.bmp",
                                    "CubeMaps/TropicalSunnyDayFront2048.bmp",
                                    "CubeMaps/TropicalSunnyDayBack2048.bmp",
                                    true);

    camera->SetPosition(glm::vec3(35.0f, 150.0f, 500.0f));

    engine.LoadSave();
    
    cMesh* skyBoxMesh = engine.LoadMesh("Sphere_1_unit_Radius_UV.ply", "skybox");
    skyBoxMesh->isSkyBox = true;
    skyBoxMesh->setUniformDrawScale(5000.0f);

    cMesh* mesh = engine.meshManager->FindMeshByFriendlyName("vampire");
    mesh->useBone = true;
    //cMesh* riggedMesh = engine.LoadMesh("Aland.FBX", "rigged");
    //riggedMesh->bDoNotLight = true;

 //   Animation* anim = new Animation("idle", mesh);
 //   AnimationInfo* animInfo = mesh->modelDrawInfo.Animations[0];
 //   
 //   for (size_t i = 0; i < animInfo->NodeAnimations.size(); i++)
 //   {
 //       NodeAnimation* nodeAnim = animInfo->NodeAnimations[i];
 //       for (size_t j = 0; j < nodeAnim->PositionKeys.size(); j++)
 //       {
 //         //  glm::vec3 position = nodeAnim->PositionKeys[j].position;
	//	//	anim->positionKeyFrames.push_back(PositionKeyFrame(nodeAnim->positionKeyFrames[j].position, nodeAnim->positionKeyFrames[j].time));
	//	}
	//}

    float currTime = 0;
    float myTime = 0;

    while (!glfwWindowShouldClose(engine.window))
    {
        engine.Update();   
    }

    engine.ShutDown();
}
