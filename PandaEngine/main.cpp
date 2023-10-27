
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
    if (!engine.LoadDefaultShaders())
    {
        return -1;
    }

    engine.SetModelPath("../Assets/Models");
    cMesh* mesh = engine.LoadMesh("bathtub_xyz_n_rgba.ply", "bathtub");
    mesh->drawPosition = glm::vec3(0.0f, -30.0f, 0.0f);

    engine.LoadDefaultLights();

    engine.Update();
}
