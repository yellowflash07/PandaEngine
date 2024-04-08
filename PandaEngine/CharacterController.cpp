#include "CharacterController.h"

extern std::map<PxActor*, PhysXBody*> gActorMap;

CharacterController::CharacterController(TransformComponent* transform)
{
	this->transform = transform;
	gPhysics = PhysXManager::getInstance()->gPhysics;
	gScene = PhysXManager::getInstance()->gScene;
	gMaterial = PhysXManager::getInstance()->gMaterial;
	manager = PxCreateControllerManager(*gScene);

	desc.height = 1.0f;
	desc.radius = 0.5f;
	desc.upDirection = PxVec3(0, 1, 0);
	desc.slopeLimit = 10.0f;
	desc.stepOffset = 1.6f;
	desc.climbingMode = PxCapsuleClimbingMode::eEASY;
	desc.material = gMaterial;
	if (!desc.isValid())
	{
		printf("Failed to create character controller\n");
		return;
	}
	manager->setOverlapRecoveryModule(true);
	controller = manager->createController(desc);
	PhysXBody* body = new PhysXBody(transform);
	body->body = controller->getActor();
	gActorMap[controller->getActor()] = body;
}

CharacterController::~CharacterController()
{
	manager->release();
}

void CharacterController::Update(float deltaTime)
{
	transform->drawPosition = glm::vec3(controller->getPosition().x, controller->getPosition().y, controller->getPosition().z);
}

void CharacterController::Move(glm::vec3 direction, float deltaTime)
{
	PxVec3 disp = PxVec3(direction.x, direction.y, direction.z) * deltaTime;
	controller->move(disp, 0.0f, deltaTime, 0);
}

void CharacterController::Render()
{
}
