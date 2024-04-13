#pragma once

#include "TransformComponent.h"
#include "PhysXManager.h"
#include "cMesh.h"
#include "IEditorUI.h"
#include <functional>
using namespace physx;

enum ColliderType
{
	NONE,
	BOX,
	SPHERE,
	MESH
};


class PhysXBody : public IEditorUI
{
public:
	PhysXBody(TransformComponent* transform);
	~PhysXBody();

	void SetShape(ColliderType type);
	void SetBody(bool isDynamic);
	void Update(float deltaTime);
	void Render() override;
	ColliderType type;
	PxShape* shape;
	bool isDynamic;
	void CreateMeshCollider(cMesh* mesh,
		bool skipMeshCleanup, bool skipEdgeData, bool inserted, const PxU32 numTrisPerLeaf);
	unsigned int uniqueID;
	std::function<void(PhysXBody* other)> onTriggerEnter;
	std::function<void(PhysXBody* other)> onTriggerExit;
	std::function<void(glm::vec3)> onContactStart;
	std::function<void(glm::vec3)> onContactEnd;
	bool isTrigger = false;
	void SetTrigger();
	PxRigidActor* body;
	void UpdateBoxDimensions(glm::vec3 halfExtents);
	void UpdateSphereDimensions(float radius);
	glm::vec3 halfExtents;
	float radius;
	cMesh* mesh;
private:
	PxMaterial* material;
	PxPhysics* gPhysics = NULL;
	PxScene* gScene = NULL;
	PxMaterial* gMaterial = NULL;
	TransformComponent* transform;
	bool setMesh = false;
	void SetupCommonCookingParams(PxCookingParams& params, bool skipMeshCleanup, bool skipEdgeData);
	bool showDebugShape = false;

};

