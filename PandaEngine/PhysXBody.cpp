#include "PhysXBody.h"
#include <iostream>

PhysXBody::PhysXBody(TransformComponent* transform)
{
	gPhysics = PhysXManager::getInstance()->gPhysics;
	gScene = PhysXManager::getInstance()->gScene;
	gMaterial = PhysXManager::getInstance()->gMaterial;
	this->transform = transform;
	SetBody(false);
	shape = NULL;
}

PhysXBody::~PhysXBody()
{
}

void PhysXBody::SetShape(ColliderType type)
{
	this->type = type;

	if (shape)
	{
		//body->detachShape(*shape);
		//shape->release();
	}

	if (type == BOX)
	{
		glm::vec3 halfExtentsVec = transform->drawScale / 2.0f;
		float halfX = halfExtentsVec.x;
		float halfY = halfExtentsVec.y;
		float halfZ = halfExtentsVec.z;
		PxVec3 halfExtents = PxVec3(halfX, halfY, halfZ);

		shape = gPhysics->createShape(PxBoxGeometry(halfExtents), *gMaterial);
	}
	else if (type == SPHERE)
	{
		shape = gPhysics->createShape(PxSphereGeometry(transform->drawScale.x), *gMaterial);
	}

	body->attachShape(*shape);
	shape->release();
}

void PhysXBody::SetBody(bool isDynamic)
{

	this->isDynamic = isDynamic;
	PxVec3 pos = PxVec3(transform->drawPosition.x, transform->drawPosition.y, transform->drawPosition.z);
	glm::quat rot = glm::quat(transform->eulerRotation);
	PxQuat rotation(rot.x, rot.y, rot.z, rot.w);
	PxTransform pxTransform = PxTransform(pos, rotation);

	if (isDynamic)
	{
		body = gPhysics->createRigidDynamic(pxTransform);
	}
	else
	{
		body = gPhysics->createRigidStatic(pxTransform);
	}
	std::string dynamic = isDynamic ? "Dynamic" : "Static";
	std::cout << "Body created: " << dynamic << std::endl;
	gScene->addActor(*body);
}

void PhysXBody::Update(float deltaTime)
{
	if(body == NULL)
		return;
	transform->drawPosition = glm::vec3(body->getGlobalPose().p.x, body->getGlobalPose().p.y, body->getGlobalPose().p.z);
	glm::quat sphereRotation = glm::quat(body->getGlobalPose().q.w, body->getGlobalPose().q.x, body->getGlobalPose().q.y, body->getGlobalPose().q.z);
	transform->eulerRotation = glm::eulerAngles(sphereRotation);
}

void PhysXBody::Render()
{
	ImGui::BeginChild("Physics Component#" , ImVec2(0, 200));
	ImGui::Text("Physics Component");
	ImGui::SetNextItemWidth(100);
	int type = this->type;
	if(ImGui::Combo("Collider", &type, "Box Collider\0Sphere Collier\0\0"))
	{
		SetShape((ColliderType)type);
	}

	ImGui::SetNextItemWidth(100);
	int dynamic = isDynamic;
	if (ImGui::Combo("Body Type", &dynamic, "Static\0Dynamic\0\0"))
	{
		gScene->removeActor(*body);
		//body->release();
		bool isDynamic = dynamic == 1;
		SetBody(isDynamic);
		SetShape(this->type);
	}

	ImGui::EndChild();
}
