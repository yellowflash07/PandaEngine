#pragma once

#include "PhysXManager.h"
#include "IEditorUI.h"
#include "PhysXBody.h"
//#include <PhysX/characterkinematic/PxControllerManager.h>
class CharacterController : public IEditorUI
{
public:
	CharacterController(TransformComponent* transform);
	~CharacterController();

	void Update(float deltaTime);

	void Move(glm::vec3 direction, float deltaTime);

	void Render() override;

	float height;
	float radius;
	glm::vec3 position;
	PxCapsuleController* controller;
	float stepOffset = 0.5f;
	float slopeLimit = 0.1f;
private:
	PxControllerManager* manager;
	PxMaterial* material;
	PxPhysics* gPhysics = NULL;
	PxScene* gScene = NULL;
	PxMaterial* gMaterial = NULL;
	TransformComponent* transform;
	PxCapsuleControllerDesc* desc;
	glm::vec3 direction;

};

