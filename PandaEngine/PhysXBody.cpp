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
	this->type = NONE;
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

		shape = gPhysics->createShape(PxBoxGeometry(halfExtents), *gMaterial, true);
	}
	else if (type == SPHERE)
	{
		shape = gPhysics->createShape(PxSphereGeometry(transform->drawScale.x), *gMaterial, true);
	}
	else if (type == MESH)
	{
		return;
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
	if(ImGui::Combo("Collider", &type, "None\0Box Collider\0Sphere Collier\0Mesh\0\0"))
	{
		SetShape((ColliderType)type);
		setMesh = false;
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

	if (ImGui::Checkbox("Trigger", &isTrigger))
	{
		SetTrigger();
	}


	ImGui::EndChild();
}

void PhysXBody::CreateMeshCollider(cMesh* mesh, bool skipMeshCleanup, bool skipEdgeData, bool inserted, const PxU32 numTrisPerLeaf)
{
	if(setMesh)
		return;
	setMesh = true;

	PxTriangleMeshDesc meshDesc;
	meshDesc.points.count = mesh->modelDrawInfo.numberOfVertices;

	PxVec3* vertices = new PxVec3[mesh->modelDrawInfo.numberOfVertices];
	for (int i = 0; i < mesh->modelDrawInfo.numberOfVertices; i++)
	{
		vertices[i] = PxVec3(mesh->modelDrawInfo.pVertices[i].x, mesh->modelDrawInfo.pVertices[i].y, mesh->modelDrawInfo.pVertices[i].z);
	}

	meshDesc.points.data = vertices;
	meshDesc.points.stride = sizeof(PxVec3);
	meshDesc.triangles.count = mesh->modelDrawInfo.numberOfTriangles;

	PxU32* indices = new PxU32[mesh->modelDrawInfo.numberOfTriangles * 3];
	for (int i = 0; i < mesh->modelDrawInfo.numberOfTriangles; i++)
	{
		indices[i * 3] = mesh->modelDrawInfo.pIndices[i * 3];
		indices[i * 3 + 1] = mesh->modelDrawInfo.pIndices[i * 3 + 1];
		indices[i * 3 + 2] = mesh->modelDrawInfo.pIndices[i * 3 + 2];
	}

	meshDesc.triangles.data = indices;
	meshDesc.triangles.stride = 3 * sizeof(PxU32);

	PxCooking *gCooking = PhysXManager::getInstance()->gCooking;
	PxCookingParams params = gCooking->getParams();

	// Create BVH34 midphase
	params.midphaseDesc = PxMeshMidPhase::eBVH33;

	if (!params.midphaseDesc.isValid()) {
		std::cerr << "Invalid midphase descriptor!" << std::endl;
		// Perform cleanup or return with an error, depending on your application flow
		return;
	}

	// setup common cooking params
	SetupCommonCookingParams(params, skipMeshCleanup, skipEdgeData);

	// Cooking mesh with less triangles per leaf produces larger meshes with better runtime performance
	// and worse cooking performance. Cooking time is better when more triangles per leaf are used.
	params.midphaseDesc.mBVH34Desc.numPrimsPerLeaf = 10.0f;

	gCooking->setParams(params);

	PX_ASSERT(gCooking->validateTriangleMesh(meshDesc));

	PxTriangleMesh* triMesh = NULL;
	PxU32 meshSize = 0;

	triMesh = gCooking->createTriangleMesh(meshDesc, gPhysics->getPhysicsInsertionCallback());

	this->shape = gPhysics->createShape(PxTriangleMeshGeometry(triMesh), *gMaterial, true);

	body->attachShape(*shape);

	triMesh->release();
}

void PhysXBody::SetupCommonCookingParams(PxCookingParams& params, bool skipMeshCleanup, bool skipEdgeData)
{

	// we suppress the triangle mesh remap table computation to gain some speed, as we will not need it 
// in this snippet
	params.suppressTriangleMeshRemapTable = true;

	// If DISABLE_CLEAN_MESH is set, the mesh is not cleaned during the cooking. The input mesh must be valid. 
	// The following conditions are true for a valid triangle mesh :
	//  1. There are no duplicate vertices(within specified vertexWeldTolerance.See PxCookingParams::meshWeldTolerance)
	//  2. There are no large triangles(within specified PxTolerancesScale.)
	// It is recommended to run a separate validation check in debug/checked builds, see below.

	if (!skipMeshCleanup)
		params.meshPreprocessParams &= ~static_cast<PxMeshPreprocessingFlags>(PxMeshPreprocessingFlag::eDISABLE_CLEAN_MESH);
	else
		params.meshPreprocessParams |= PxMeshPreprocessingFlag::eDISABLE_CLEAN_MESH;

	// If DISABLE_ACTIVE_EDGES_PREDOCOMPUTE is set, the cooking does not compute the active (convex) edges, and instead 
	// marks all edges as active. This makes cooking faster but can slow down contact generation. This flag may change 
	// the collision behavior, as all edges of the triangle mesh will now be considered active.
	if (!skipEdgeData)
		params.meshPreprocessParams &= ~static_cast<PxMeshPreprocessingFlags>(PxMeshPreprocessingFlag::eDISABLE_ACTIVE_EDGES_PRECOMPUTE);
	else
		params.meshPreprocessParams |= PxMeshPreprocessingFlag::eDISABLE_ACTIVE_EDGES_PRECOMPUTE;

}

void PhysXBody::SetTrigger()
{
	if (isTrigger)
	{
		shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
		shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);
	}
	else
	{
		shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, false);
		shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, true);
	}
}
