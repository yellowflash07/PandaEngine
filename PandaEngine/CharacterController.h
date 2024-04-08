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

private:
	PxController* controller;
	PxControllerManager* manager;
	PxMaterial* material;
	PxPhysics* gPhysics = NULL;
	PxScene* gScene = NULL;
	PxMaterial* gMaterial = NULL;
	TransformComponent* transform;
	PxCapsuleControllerDesc desc;
};

