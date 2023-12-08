
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

    cMesh* sphere = engine.LoadMesh("Sphere_1_unit_Radius_UV.ply", "sphere");
    sphere->drawPosition = glm::vec3(0.0f, 60.0f, 0.0f);
    camera->cameraEye = glm::vec3(0.0f, 60.0f, 0.0f);
    PhysicsBody* body = engine.AddPhysicsBody("sphere");
    body->inverseMass = 1.0f/10.0f;
   // body->inverseMass = 0.0f;
    body->acceleration = glm::vec3(0.0f, -9.8f/50.0f, 0.0f);
    body->shapeType = PhysicsShapes::SPHERE;
    body->setShape(new PhysicsShapes::sSphere(1.0f));

  //  cMesh* ground = engine.LoadMesh("bathtub_xyz_n_rgba.ply", "ground");
  //  ground->transperancy = 1.0f;
  //  ground->bDoNotLight = true;
  ////  ground->texture[0] = "PaletteV1.bmp";
  //  ground->textureRatio[0] = 1.0f;
  //  ground->drawPosition = glm::vec3(0.0f, 0.0f, 0.0f);
   // ground->setRotationFromEuler(glm::vec3(-1.6f, 0.0f, 0.0f));

    //PhysicsBody* body1 = engine.AddPhysicsBody("ground");
    //body1->inverseMass = 0;
    //body1->shapeType = PhysicsShapes::MESH_OF_TRIANGLES_INDIRECT;
    //body1->setShape(new PhysicsShapes::sMeshOfTriangles_Indirect("ground"));


    engine.physicsManager->GenerateAABBs(body, 1);

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
    
    engine.physicsManager->GenerateAABBs(body1,10);


    std::vector<cMesh*> sphereAABBmeshes;
    std::vector<cMesh*> meshes;

    for (size_t i = 0; i < body->aabbs.size(); i++)
    {
        cMesh* mesh = engine.LoadMesh("Cube_1x1x1_xyz_n_rgba.ply", "SPHEREAABB" + std::to_string(i));
        //mesh->color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
        mesh->drawScale = body->aabbs[i]->getExtentsXYZ();
        mesh->drawPosition = body->aabbs[i]->getCentreXYZ();
        mesh->bDoNotLight = true;
        mesh->bUseDebugColours = true;
        mesh->wholeObjectDebugColourRGBA = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
        mesh->drawPosition = body->aabbs[i]->getCentreXYZ();
        mesh->bIsWireframe = true;
        sphereAABBmeshes.push_back(mesh);
    }

    int count = 0;
    for (size_t i = 0; i < body1->aabbs.size(); i++)
    {

        cMesh* mesh = engine.LoadMesh("Cube_1x1x1_xyz_n_rgba.ply", "AABB" + std::to_string(i));
        //mesh->color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
        mesh->drawScale = body1->aabbs[i]->getExtentsXYZ();
        mesh->drawPosition = body1->aabbs[i]->getCentreXYZ();
        mesh->bIsVisible = true;
        mesh->bDoNotLight = true;
        mesh->bUseDebugColours = true;
        mesh->wholeObjectDebugColourRGBA = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
        mesh->bIsWireframe = true;
        meshes.push_back(mesh);
   /*     if (!body1->aabbs[i]->vecTrianglesInside.empty())
        {
            mesh->bIsWireframe = false;
        }*/
      //  std::cout << "Triangles inside: " << "AABB" + std::to_string(i) << " :" << body1->aabbs[i]->vecTrianglesInside.size() << std::endl;
       // count += body1->aabbs[i]->vecTrianglesInside.size();
    }


    engine.LoadDefaultLights();

    while (!glfwWindowShouldClose(engine.window))
    {
        engine.Update();
        skyBoxMesh->drawPosition = camera->cameraEye;

        if (keyHit == GLFW_KEY_UP)
        {
			body->velocity.z += 0.1f;
		}
        if (keyHit == GLFW_KEY_DOWN)
        {
            body->velocity.z -= 0.1f;
        }
        if (keyHit == GLFW_KEY_LEFT)
        {
			body->velocity.x -= 0.1f;
		}
        if (keyHit == GLFW_KEY_RIGHT)
        {
            body->velocity.x += 0.1f;
        }


   //     for (size_t i = 0; i < body->aabbs.size(); i++)
   //     {
   //         sphereAABBmeshes[i]->drawPosition = body->aabbs[i]->getCentreXYZ();
   //         if(body->aabbs[i]->isOverlapping)
   //             sphereAABBmeshes[i]->wholeObjectDebugColourRGBA = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
			//else
   //             sphereAABBmeshes[i]->wholeObjectDebugColourRGBA = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);

   //     }
   //     for (size_t i = 0; i < body1->aabbs.size(); i++)
   //     {
   //         if (body1->aabbs[i]->isOverlapping)
   //         {
   //             meshes[i]->wholeObjectDebugColourRGBA = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
   //             meshes[i]->bIsVisible = true;
   //         //    std::cout << "Overlapping" << meshes[i]->friendlyName << std::endl;
   //         }             
   //         else
   //         {
   //             meshes[i]->wholeObjectDebugColourRGBA = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
			//	meshes[i]->bIsVisible = false;
   //         }

   //     }
    }

    engine.ShutDown();
}
