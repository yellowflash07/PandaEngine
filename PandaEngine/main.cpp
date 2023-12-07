
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

    cMesh* city = engine.LoadMesh("cartoonCity_Showcase_rgba.ply", "city");
    city->transperancy = 1.0f;
    city->bDoNotLight = true;
    city->texture[0] = "PaletteV1.bmp";
    city->textureRatio[0] = 1.0f;
    city->drawPosition = glm::vec3(0.0f, 0.0f, 0.0f);
    city->setRotationFromEuler(glm::vec3(-1.6f, 0.0f, 0.0f));

    PhysicsBody* body1 = engine.AddPhysicsBody("city");
    body1->inverseMass = 0;
    body1->shapeType = PhysicsShapes::MESH_OF_TRIANGLES_INDIRECT;
    body1->setShape(new PhysicsShapes::sMeshOfTriangles_Indirect("city"));
    
    std::vector<cAABB*> aabbs = engine.physicsManager->GenerateAABBs(body1,10);

   // city->calcExtents();

    cMesh* mesh = engine.LoadMesh("Cube_1x1x1_xyz_n_rgba.ply", "min");
    mesh->color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
    glm::vec3 extents = city->maxExtents_XYZ - city->minExtents_XYZ;
    mesh->drawScale = extents;
    mesh->drawPosition = (city->minExtents_XYZ + city->maxExtents_XYZ) * 0.5f;
    mesh->bIsWireframe = true;


    for (size_t i = 0; i < aabbs.size(); i++)
    {
        cMesh* mesh = engine.LoadMesh("Cube_1x1x1_xyz_n_rgba.ply", "AABB" + std::to_string(i));
        mesh->color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
        mesh->drawScale = aabbs[i]->getExtentsXYZ();
        mesh->drawPosition = aabbs[i]->getCentreXYZ();
        mesh->bIsWireframe = true;
    }

    engine.LoadDefaultLights();

    while (!glfwWindowShouldClose(engine.window))
    {
        engine.Update();
        skyBoxMesh->drawPosition = camera->cameraEye;
    }

    engine.ShutDown();
}
