#include "CharacterController.h"
#include <algorithm>
extern std::map<PxActor*, PhysXBody*> gActorMap;

CharacterController::CharacterController(TransformComponent* transform)
{
	this->transform = transform;
	gPhysics = PhysXManager::getInstance()->gPhysics;
	gScene = PhysXManager::getInstance()->gScene;
	gMaterial = PhysXManager::getInstance()->gMaterial;
	manager = PxCreateControllerManager(*gScene);
	desc = new PxCapsuleControllerDesc();
	desc->height = 1.0f;
	desc->radius = 0.5f;
	this->radius = 0.5f;
	this->height = 1.0f;
	//desc->position = PxExtendedVec3(transform->drawPosition.x, transform->drawPosition.y, transform->drawPosition.z);
	//this->position = transform->drawPosition;
	desc->upDirection = PxVec3(0, 1, 0);
	desc->slopeLimit = 10000.0f;
	desc->stepOffset = 1.6f;
	desc->climbingMode = PxCapsuleClimbingMode::eEASY;
	desc->material = gMaterial;
	if (!desc->isValid())
	{
		printf("Failed to create character controller\n");
		EXIT_FAILURE;
		return;
	}
	manager->setOverlapRecoveryModule(true);

	controller = (PxCapsuleController*)manager->createController(*desc);
	//PhysXBody* physXBody = new PhysXBody(transform);
	//physXBody->body = controller->getActor();
	//controller->getActor()->userData = physXBody;

	PhysXBody* body = new PhysXBody(transform);
	body->body->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, false);
	body->body = controller->getActor();
	gActorMap[controller->getActor()] = body;
	PhysXManager::getInstance()->updateDebug = true;

	this->direction = glm::vec3(0.0f, 0.0f, 0.0f);
}

CharacterController::~CharacterController()
{
	manager->release();
}

void CharacterController::Update(float deltaTime)
{
	if (isHolding) return;

	// Get current vertical velocity
	PxVec3 currentVelocity = controller->getActor()->getLinearVelocity();
	float currentYVelocity = currentVelocity.y;

	// Add gravity to vertical velocity
	currentYVelocity += PhysXManager::getInstance()->gScene->getGravity().y * deltaTime;

	// Clamp vertical velocity (optional)
	//currentYVelocity = std::clamp(currentYVelocity, -10.0f, 10.0f);  // Adjust terminal velocity as needed

	// Combine direction (assumed to be available) and updated vertical velocity
	PxVec3 disp = PxVec3(direction.x, currentYVelocity, direction.z) * deltaTime;

	// Move the character controller (assuming direction is set elsewhere)
	controller->move(disp, 0.0f, deltaTime, 0);

//	PxTransform poseTransform = PxShapeExt::getGlobalPose(controller->getActor()->getGlobalPose(), *gVehicle4W->getRigidDynamicActor());

	// Update character's draw position based on actual position after move
	transform->drawPosition = glm::vec3(controller->getFootPosition().x, controller->getFootPosition().y, controller->getFootPosition().z);
}

void CharacterController::Move(glm::vec3 direction, float deltaTime)
{
	this->direction = direction;
	//PxVec3 disp = PxVec3(direction.x, direction.y, direction.z) * deltaTime;
	//// Move the character controller
	//controller->move(disp, 0.0f, deltaTime, 0);
}

void CharacterController::Hold(glm::vec3 position)
{
	controller->setPosition(PxExtendedVec3(position.x, position.y, position.z));
	isHolding = true;
}

void CharacterController::StopHold()
{
	isHolding = false;
}

void CharacterController::Render()
{
	ImGui::BeginChild("Character Controller");
	ImGui::SeparatorText("Character Controller");

	if (ImGui::DragFloat("Radius", &radius, 0.1f))
	{
		controller->setRadius(radius);
		PhysXManager::getInstance()->updateDebug = true;
	}
	if (ImGui::DragFloat("Height", &height, 0.1f))
	{
		controller->setHeight(height);
		PhysXManager::getInstance()->updateDebug = true;
	}

	if (ImGui::DragFloat("Slope Limit", &slopeLimit, 0.1f))
	{
		controller->setSlopeLimit(slopeLimit);
	}
	if (ImGui::DragFloat("Step Offset", &stepOffset, 0.1f))
	{
		controller->setStepOffset(stepOffset);
	}

	if (ImGui::DragFloat3("Position", &position[0], 0.1f))
	{
		controller->setPosition(PxExtendedVec3(position.x, position.y, position.z));
		PhysXManager::getInstance()->updateDebug = true;
	};

	ImGui::EndChild();
}
