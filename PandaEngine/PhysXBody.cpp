#include "PhysXBody.h"
#include <iostream>
#include "VehicleCreator.h"

std::map<PxActor*, PhysXBody*> gActorMap;


PhysXBody::PhysXBody(TransformComponent* transform)
{
	gPhysics = PhysXManager::getInstance()->gPhysics;
	gScene = PhysXManager::getInstance()->gScene;
	//gMaterial = gPhysics->createMaterial(0.0f, 0.0f, 0.0f);
	gMaterial = PhysXManager::getInstance()->gMaterial;
	this->transform = transform;
	SetBody(false);
	shape = NULL;
	this->type = NONE;

	this->uniqueID = rand() % 10000 + 1;
	PhysXManager::getInstance()->updateDebug = true;
}

PhysXBody::~PhysXBody()
{
	::gActorMap.erase(body);
	gScene->removeActor(*body);
	body->release();
	if (shape)
	{
	//	shape->re
	}
}

void PhysXBody::SetShape(ColliderType type)
{
	this->type = type;

	PxMaterial* surfaceMaterial = PhysXManager::getInstance()->gMaterial;

	if (type == BOX)
	{
		glm::vec3 halfExtentsVec = transform->drawScale / 2.0f;
		float halfX = halfExtentsVec.x;
		float halfY = halfExtentsVec.y;
		float halfZ = halfExtentsVec.z;
		PxVec3 halfExtents = PxVec3(halfX, halfY, halfZ);
		this->halfExtents = halfExtentsVec;
		shape = gPhysics->createShape(PxBoxGeometry(halfExtents), *surfaceMaterial, true);
		shape->setFlag(PxShapeFlag::eVISUALIZATION, true);

	}
	else if (type == SPHERE)
	{
		this->radius = transform->drawScale.x;
		shape = gPhysics->createShape(PxSphereGeometry(transform->drawScale.x), *surfaceMaterial, true);
		shape->setFlag(PxShapeFlag::eVISUALIZATION, true);

	}
	else if (type == MESH)
	{
		shape = CreateMeshCollider(mesh, false, false, false, 1);
		shape->setFlag(PxShapeFlag::eVISUALIZATION, false);

	}
	PxFilterData objectSimFilterData(COLLISION_FLAG_OBSTACLE, COLLISION_FLAG_WHEEL, PxPairFlag::eMODIFY_CONTACTS | PxPairFlag::eDETECT_CCD_CONTACT, 0);
	shape->setSimulationFilterData(objectSimFilterData);
	PxFilterData qryFilterData;
	qryFilterData.word3 = DRIVABLE_SURFACE;
	shape->setQueryFilterData(qryFilterData);
	body->attachShape(*shape);
	shape->userData = this;
	body->userData = this;
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
		//body->is<PxRigidDynamic>()->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);
	}
	else
	{
		body = gPhysics->createRigidStatic(pxTransform);
	}
	std::string dynamic = isDynamic ? "Dynamic" : "Static";
	//std::cout << "Body created: " << dynamic << std::endl;
	gScene->addActor(*body);
	gActorMap[this->body] = this;
	body->userData = this;
	body->setActorFlag(PxActorFlag::eVISUALIZATION, true);
}

void PhysXBody::Update(float deltaTime)
{
	if(body == NULL)
		return;

	if (!isDynamic)
	{
		return;
	}
	
	PxTransform pxTransform = PxShapeExt::getGlobalPose(*shape, *body);
	transform->drawPosition = glm::vec3(pxTransform.p.x, pxTransform.p.y, pxTransform.p.z);
	glm::quat sphereRotation = glm::quat(pxTransform.q.w, pxTransform.q.x, pxTransform.q.y, pxTransform.q.z);
	transform->eulerRotation = glm::eulerAngles(sphereRotation);
}

void PhysXBody::Render()
{
	ImGui::BeginChild("Physics Component#" , ImVec2(0, 200));
	ImGui::SeparatorText("Physics Component");
	int type = this->type;
	if(ImGui::Combo("Collider", &type, "None\0Box Collider\0Sphere Collier\0Mesh\0\0"))
	{
		SetShape((ColliderType)type);
		setMesh = false;
		//PhysXManager::getInstance()->updateDebug = true;
	}

	if (type == BOX)
	{
		
		if (ImGui::DragFloat3("Half Extents", &halfExtents[0], 0.1f))
		{
			UpdateBoxDimensions(halfExtents);
			PhysXManager::getInstance()->updateDebug = true;
		}
		if (ImGui::DragFloat3("Position", &position[0], 0.1f))
		{
			shape->setLocalPose(PxTransform(position.x, position.y, position.z));
			PhysXManager::getInstance()->updateDebug = true;
		}
	}
	else if (type == SPHERE)
	{
		if (ImGui::DragFloat("Radius", &radius, 0.1f))
		{
			UpdateSphereDimensions(radius);
			PhysXManager::getInstance()->updateDebug = true;
		}
		if (ImGui::DragFloat3("Position", &position[0], 0.1f))
		{
			shape->setLocalPose(PxTransform(position.x, position.y, position.z));
			PhysXManager::getInstance()->updateDebug = true;
		}
	}

	int dynamic = isDynamic;
	if (ImGui::Combo("Body Type", &dynamic, "Static\0Dynamic\0\0"))
	{
		::gActorMap.erase(body);
		gScene->removeActor(*body);
		//body->release();
		bool isDynamic = dynamic == 1;
		SetBody(isDynamic);
		SetShape(this->type);
		PhysXManager::getInstance()->updateDebug = true;
	}

	if (ImGui::Checkbox("Trigger", &isTrigger))
	{
		SetTrigger();
	}
	

	ImGui::Separator();
	ImGui::EndChild();
}

PxShape* PhysXBody::CreateMeshCollider(cMesh* mesh, bool skipMeshCleanup, bool skipEdgeData, bool inserted, const PxU32 numTrisPerLeaf)
{

	PxTriangleMeshDesc meshDesc;
	sModelDrawInfo modelDrawInfo;
	mesh->GetTransformedMeshDrawInfo(modelDrawInfo);
	meshDesc.points.count = modelDrawInfo.numberOfVertices;

	PxVec3* vertices = new PxVec3[modelDrawInfo.numberOfVertices];
	for (int i = 0; i < modelDrawInfo.numberOfVertices; i++)
	{
		vertices[i] = PxVec3(modelDrawInfo.pVertices[i].x, modelDrawInfo.pVertices[i].y, modelDrawInfo.pVertices[i].z);
	}

	meshDesc.points.data = vertices;
	meshDesc.points.stride = sizeof(PxVec3);
	meshDesc.triangles.count = modelDrawInfo.numberOfTriangles;

	PxU32* indices = new PxU32[modelDrawInfo.numberOfTriangles * 3];
	for (int i = 0; i < modelDrawInfo.numberOfTriangles; i++)
	{
		indices[i * 3] = modelDrawInfo.pIndices[i * 3];
		indices[i * 3 + 1] = modelDrawInfo.pIndices[i * 3 + 1];
		indices[i * 3 + 2] = modelDrawInfo.pIndices[i * 3 + 2];
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
		return nullptr;
	}

	// setup common cooking params
	SetupCommonCookingParams(params, skipMeshCleanup, skipEdgeData);

	// Cooking mesh with less triangles per leaf produces larger meshes with better runtime performance
	// and worse cooking performance. Cooking time is better when more triangles per leaf are used.
	params.midphaseDesc.mBVH34Desc.numPrimsPerLeaf = 10.0f;

	gCooking->setParams(params);

	PX_ASSERT(gCooking->validateTriangleMesh(meshDesc));


	PxTriangleMesh* triMesh = gCooking->createTriangleMesh(meshDesc, gPhysics->getPhysicsInsertionCallback());
	
	PxShape* shape = gPhysics->createShape(PxTriangleMeshGeometry(triMesh), *gMaterial, true);

	triMesh->release();

	delete [] vertices;
	delete [] indices;

	return shape;
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

void PhysXBody::UpdateBoxDimensions(glm::vec3 halfExtents)
{
//	this->shape->getGeometry().box().halfExtents = PxVec3(halfExtents.x, halfExtents.y, halfExtents.z);
	PxBoxGeometry boxGeometry;
	this->shape->getBoxGeometry(boxGeometry);
	boxGeometry.halfExtents = PxVec3(halfExtents.x, halfExtents.y, halfExtents.z);
	this->shape->setGeometry(boxGeometry);
}

void PhysXBody::UpdateSphereDimensions(float radius)
{
	PxSphereGeometry sphereGeometry;
	this->shape->getSphereGeometry(sphereGeometry);
	sphereGeometry.radius = radius;
	this->shape->setGeometry(sphereGeometry);
}

void PhysXBody::SetDriveableGround()
{
	PxFilterData groundPlaneSimFilterData(COLLISION_FLAG_GROUND, COLLISION_FLAG_WHEEL, 0, 0);
	shape->setSimulationFilterData(groundPlaneSimFilterData);
	PxFilterData qryFilterData;
	qryFilterData.word3 = DRIVABLE_SURFACE;
	shape->setQueryFilterData(qryFilterData);
}

void PhysXBody::SetDriveableObject()
{
	PxFilterData objectSimFilterData(COLLISION_FLAG_OBSTACLE, COLLISION_FLAG_WHEEL, PxPairFlag::eMODIFY_CONTACTS | PxPairFlag::eDETECT_CCD_CONTACT, 0);
	shape->setSimulationFilterData(objectSimFilterData);
	PxFilterData qryFilterData;
	qryFilterData.word3 = DRIVABLE_SURFACE;
	shape->setQueryFilterData(qryFilterData);
}

