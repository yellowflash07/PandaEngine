
#include "Engine.h"
#include "Random.h"
#include <iostream>

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

    //bathtub_xyz_n_rgba
    //Terrain_xyz_n_rgba_uv
    cMesh* groundMesh = engine.LoadMesh("bathtub_xyz_n_rgba.ply", "bathtub");
   // groundMesh->texture = "Water_Texture_01.bmp";
    groundMesh->transperancy = 0.03f;
    groundMesh->bDoNotLight = false;
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
    }

    engine.ShutDown();
}
