
#include "Engine.h"
#include <iostream>

int main(void)
{
    Engine engine;
    if (!engine.Initialize())
    {
        return 1;
    }
    engine.SetShaderPath("../Assets/Shaders");
    engine.SetModelPath("../Assets/Models");

    if (!engine.LoadDefaultShaders())
    {
        return 2;
    }

    cMesh* groundMesh = engine.LoadMesh("bathtub_xyz_n_rgba.ply", "bathtub");
    groundMesh->drawPosition = glm::vec3(0.0f, -30.0f, 0.0f);
    PhysicsBody* body1 = engine.AddPhysicsBody("bathtub");
    body1->inverseMass = 0;
    body1->shapeType = PhysicsShapes::MESH_OF_TRIANGLES_INDIRECT;
    body1->setShape(new PhysicsShapes::sMeshOfTriangles_Indirect("bathtub"));

    cMesh* sphereMesh = engine.LoadMesh("Sphere_1_unit_Radius.ply", "Sphere");
    sphereMesh->drawPosition = glm::vec3(0.0f, 0.0f, 0.0f);

    PhysicsBody* body = engine.AddPhysicsBody("Sphere");
    body->shapeType = PhysicsShapes::SPHERE;
    body->setShape(new PhysicsShapes::sSphere(1.0));
    body->acceleration = glm::vec3(0, -9.8f/5, 0);
    body->inverseMass = 1/10;

    engine.LoadDefaultLights();

    while (!glfwWindowShouldClose(engine.window))
    {
        engine.Update();
    }

    engine.ShutDown();
}
