
#include "Engine.h"
#include "Random.h"
#include <iostream>
#include <map>
#include "ConfigReader.h"
#include "AssetLibrary.h"

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

    camera->SetPosition(glm::vec3(0.0f, 400.0f, 200.0f));
    
    cMesh* skyBoxMesh = engine.LoadMesh("Sphere_1_unit_Radius_UV.ply", "skybox");
    skyBoxMesh->isSkyBox = true;
    skyBoxMesh->setUniformDrawScale(5000.0f);

    cMesh* tieFighter = engine.LoadMesh("tie_fighter.ply", "TieFighter", false);
    tieFighter->drawPosition = glm::vec3(0.0f, -50.0f, 0.0f);

    std::vector<cMesh*> offScreenMeshList;
    offScreenMeshList.push_back(tieFighter);

    cMesh* mesh = engine.LoadMesh("Terrain_xyz_n_rgba_uv.ply", "flat_plane");
    mesh->bDoNotLight = true;
    Camera* rtCam = new Camera(glm::vec3(0.0f, 400.0f, 200.0f),
                                glm::vec3(0.0f, 0.0f, -1.0f),
                                glm::vec3(0.0f, 1.0f, 0.0f), 0.1f, 10000.0f);

    RenderTexture* rt = engine.CreateRenderTexture(rtCam, offScreenMeshList);

    float currTime = 0;
    float myTime = 0;

    while (!glfwWindowShouldClose(engine.window))
    {
        engine.Update();   
        mesh->renderTextureID = rt->GetTextureID();
    }

    engine.ShutDown();
}
