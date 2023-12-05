
#include "Engine.h"
#include "Random.h"
#include <iostream>

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

int main(void)
{
    Engine engine;
    if (!engine.Initialize())
    {
        return 1;
    }

    engine.meshManager->LoadTexture("Water_Texture_01.bmp");
    engine.meshManager->LoadTexture("TaylorSwift_Eras_Poster.bmp");
    engine.meshManager->LoadTexture("FAKE_Stencil_Texture_612x612.bmp");
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

    //bathtub_xyz_n_rgba
    //Terrain_xyz_n_rgba_uv
    cMesh* groundMesh = engine.LoadMesh("Terrain_xyz_n_rgba_uv.ply", "bathtub");
    groundMesh->texture[0] = "Water_Texture_01.bmp";
    groundMesh->texture[1] = "TaylorSwift_Eras_Poster.bmp";
    groundMesh->maskTexture = "FAKE_Stencil_Texture_612x612.bmp";
    groundMesh->textureRatio[0] = 0.5f;
    groundMesh->textureRatio[1] = 0.5f;

    groundMesh->transperancy = 1.0f;
    groundMesh->bDoNotLight = true;
    groundMesh->drawPosition = glm::vec3(0.0f, -30.0f, 0.0f);
    PhysicsBody* body1 = engine.AddPhysicsBody("bathtub");
    body1->inverseMass = 0;
    body1->shapeType = PhysicsShapes::MESH_OF_TRIANGLES_INDIRECT;
    body1->setShape(new PhysicsShapes::sMeshOfTriangles_Indirect("bathtub"));

    //cMesh* sphereMesh = engine.LoadMesh("Sphere_1_unit_Radius.ply", "Sphere");
    //sphereMesh->drawPosition = glm::vec3(0.0f, 0.0f, 0.0f);

    //PhysicsBody* body = engine.AddPhysicsBody("Sphere");
    //body->shapeType = PhysicsShapes::SPHERE;
    //body->setShape(new PhysicsShapes::sSphere(1.0));
    //body->acceleration = glm::vec3(0, -9.8f/5, 0);
    //body->inverseMass = 1.0f/10.0f;

    engine.LoadDefaultLights();

    while (!glfwWindowShouldClose(engine.window))
    {
        engine.Update();
        skyBoxMesh->drawPosition = camera->cameraEye;
    }

    engine.ShutDown();
}
