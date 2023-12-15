#pragma once
#include "Command.h"
#include <glm/glm.hpp>
#include <functional>
#include "Engine.h"

class LocationTrigger :
    public Command
{
public:
    LocationTrigger();
    virtual ~LocationTrigger() {};
    virtual bool Execute(double deltaTime);
    virtual void SetParams(Engine engine, cMesh* meshToTrigger, glm::vec3 location, float size);

    std::function<void()> OnEnter;
private:
    cAABB* aabb;
};

