
#include "Engine.h"
#include "Random.h"
#include <iostream>
#include "AnimationSystem.h"
//#include <px>

#include <PhysX/PxPhysicsAPI.h>
#include <PhysX/pvd/PxPvd.h>

extern Camera* camera;
int keyHit = 0;

#include "Scene.h"

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

using namespace physx;
PxDefaultAllocator		gAllocator;
PxDefaultErrorCallback	gErrorCallback;

PxFoundation* gFoundation = NULL;
PxPhysics* gPhysics = NULL;

PxDefaultCpuDispatcher* gDispatcher = NULL;
PxScene* gScene = NULL;

PxMaterial* gMaterial = NULL;

PxPvd* gPvd = NULL;

PxReal stackZ = 10.0f;

#define PVD_HOST "127.0.0.1"

void createStack(const PxTransform& t, PxU32 size, PxReal halfExtent)
{
    
    PxShape* shape = gPhysics->createShape(PxBoxGeometry(halfExtent, halfExtent, halfExtent), *gMaterial);
    for (PxU32 i = 0; i < size; i++)
    {
        for (PxU32 j = 0; j < size - i; j++)
        {
            PxTransform localTm(PxVec3(PxReal(j * 2) - PxReal(size - i), PxReal(i * 2 + 1), 0) * halfExtent);
            PxRigidDynamic* body = gPhysics->createRigidDynamic(t.transform(localTm));
            body->attachShape(*shape);
            PxRigidBodyExt::updateMassAndInertia(*body, 10.0f);
            gScene->addActor(*body);
        }
    }
    shape->release();
}

void initPhysics(bool interactive)
{
    gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);

    gPvd = PxCreatePvd(*gFoundation);
    PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);
    gPvd->connect(*transport, PxPvdInstrumentationFlag::eALL);

    gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale(), true, gPvd);

    PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
    sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
    gDispatcher = PxDefaultCpuDispatcherCreate(2);
    sceneDesc.cpuDispatcher = gDispatcher;
    sceneDesc.filterShader = PxDefaultSimulationFilterShader;
    gScene = gPhysics->createScene(sceneDesc);

    PxPvdSceneClient* pvdClient = gScene->getScenePvdClient();
    if (pvdClient)
    {
        pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
        pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
        pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
    }
    gMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.6f);
  
}

void stepPhysics(/*bool interactive*/)
{
    gScene->simulate(1.0f / 60.0f);
    gScene->fetchResults(true);
}

physx::PxTransform ConvertMat4ToPxTransform(const glm::mat4& matrix)
{
    glm::vec3 translation = glm::vec3(matrix[3]); // Extract translation component

    // Extract rotation component
    glm::mat3 rotationMatrix = glm::mat3(matrix);
    physx::PxQuat rotation;
    rotation.x = rotationMatrix[0][0];
    rotation.y = rotationMatrix[1][0];
    rotation.z = rotationMatrix[2][0];
    rotation.w = 1.0f + rotationMatrix[0][0] + rotationMatrix[1][1] + rotationMatrix[2][2];
    rotation.w = sqrtf(rotation.w) * 0.5f;

    // Create PxTransform
    physx::PxTransform transform;
    transform.p = physx::PxVec3(translation.x, translation.y, translation.z);
    transform.q = rotation;

    return transform;
}


int main(void)
{
    Engine engine;
    if (!engine.Initialize())
    {
        return 1;
    }

    bool loaded = engine.meshManager->LoadCubeMap("space",
                                    "CubeMaps/TropicalSunnyDayLeft2048.bmp",
                                    "CubeMaps/TropicalSunnyDayRight2048.bmp",
                                    "CubeMaps/TropicalSunnyDayDown2048.bmp",
                                    "CubeMaps/TropicalSunnyDayUp2048.bmp",
                                    "CubeMaps/TropicalSunnyDayFront2048.bmp",
                                    "CubeMaps/TropicalSunnyDayBack2048.bmp",
                                    true);

    camera->SetPosition(glm::vec3(11.0f,158.0f, 401.0f));

    engine.LoadSave(); 

    float currTime = 0;
    float myTime = 0;

    initPhysics(true);

    Scene* scene = engine.GetCurrentScene();
    GameObject* sphere = scene->GetGameObjectByName("Sphere");
    TransformComponent* sphereTransform = sphere->GetComponent<TransformComponent>();
    cMesh* sphereMesh = sphere->GetComponent<cMesh>();
    sphereMesh->calcExtents();
    float radius = sphereTransform->drawScale.x/2.0f;
    PxShape* shape = gPhysics->createShape(PxSphereGeometry(radius), *gMaterial);

    PxTransform t = PxTransform(PxVec3(sphereTransform->drawPosition.x, 
        sphereTransform->drawPosition.y, sphereTransform->drawPosition.z));

    PxRigidDynamic* body = gPhysics->createRigidDynamic(t);
    body->attachShape(*shape);
    gScene->addActor(*body);
    shape->release();

    GameObject* debugSphere = scene->GetGameObjectByName("DebugSphere");
    cMesh* dsMesh =  debugSphere->GetComponent<cMesh>();
    dsMesh->bIsWireframe = true;
    TransformComponent* debugSphereTransform = debugSphere->GetComponent<TransformComponent>();
    debugSphereTransform->drawPosition = glm::vec3(t.p.x, t.p.y, t.p.z);
    debugSphereTransform->drawScale = glm::vec3(radius * 2.0f);


    GameObject* plane = scene->GetGameObjectByName("Plane");
    TransformComponent* planeTransform = plane->GetComponent<TransformComponent>();
    cMesh* planeMesh = plane->GetComponent<cMesh>();
    planeMesh->calcExtents();
  //  glm::vec3 halfExtentsVec = (planeMesh->maxExtents_XYZ - planeMesh->minExtents_XYZ)/2.0f;
    glm::vec3 halfExtentsVec = planeTransform->drawScale/2.0f;
    float halfX = halfExtentsVec.x;
    float halfY = halfExtentsVec.y;
    float halfZ = halfExtentsVec.z;
    PxVec3 halfExtents = PxVec3(halfX, halfY, halfZ);

    GameObject* debugCube = scene->GetGameObjectByName("DebugCube");
    TransformComponent* dcT = debugCube->GetComponent<TransformComponent>();
    //dcT->drawPosition = planeTransform->drawPosition;
   // dcT->drawScale = halfExtentsVec * 2.0f;
    cMesh* dcM = debugCube->GetComponent<cMesh>();
    dcM->bIsWireframe = true;
    dcM->bUseDebugColours = true;
    dcM->wholeObjectDebugColourRGBA = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);

    PxShape* groundShape = gPhysics->createShape(PxBoxGeometry(halfExtents), *gMaterial);
   
    camera->speed = 150.0f;
   // PxTransform localTransform;
   // glm::vec3 position = planeMesh->GetCenter();
    glm::vec3 position = planeTransform->drawPosition;
    PxVec3 positionPX (position.x, position.y + 2.7f, position.z);
    glm::quat rot = glm::quat(planeTransform->eulerRotation);
    PxQuat rotation    (rot.x,
                            rot.y,
                            rot.z,
                            rot.w);

    PxTransform localTransform (positionPX, rotation);

    dcT->drawPosition = glm::vec3(localTransform.p.x, localTransform.p.y, localTransform.p.z);
    dcT->drawScale = halfExtentsVec * 2.0f;
    dcT->eulerRotation = glm::eulerAngles(glm::quat(localTransform.q.w, localTransform.q.x, localTransform.q.y, localTransform.q.z));

 //   groundShape->setLocalPose(localTransform);
    PxRigidStatic* groundPlane = gPhysics->createRigidStatic(localTransform);
    groundPlane->attachShape(*groundShape);
    gScene->addActor(*groundPlane);
    groundShape->release();

   // PxRigidStatic* groundPlane = PxCreatePlane(*gPhysics, PxPlane(0, 1, 0, 0), *gMaterial);
   // gScene->addActor(*groundPlane);

    while (!glfwWindowShouldClose(engine.window))
    {

        engine.BeginRender();

        engine.Update();

        stepPhysics();
        sphereTransform->drawPosition = glm::vec3(body->getGlobalPose().p.x, body->getGlobalPose().p.y, body->getGlobalPose().p.z);
        glm::quat sphereRotation = glm::quat(body->getGlobalPose().q.w, body->getGlobalPose().q.x, body->getGlobalPose().q.y, body->getGlobalPose().q.z);
        sphereTransform->eulerRotation = glm::eulerAngles(sphereRotation);

        engine.EndRender();

      

        
    }

    engine.ShutDown();
}
