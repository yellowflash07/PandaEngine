#include "PhysicsManager.h"
#include <iostream>

PhysicsManager::PhysicsManager()
{
}

PhysicsManager::PhysicsManager(MeshManager* meshManager)
{
	this->meshManager = meshManager;
}

PhysicsManager::~PhysicsManager()
{
}

void PhysicsManager::Update(float deltaTime)
{
	CheckIntersections(deltaTime);
}

void PhysicsManager::CheckIntersections(float deltaTime)
{
	for (PhysicsBody* pObject : this->bodies)
	{
		// Infinite mass? 
		if (pObject->inverseMass >= 0.0f)
		{
			// Velocity change is based on the acceleration over this time frame 
			// This part: (Accel * DeltaTime)
			glm::vec3 deltaVelocityThisFrame = pObject->acceleration * (float)deltaTime;

			// Update the velocity based on this delta velocity
			// Then this part: NewVelocity = LastVel + ...
			pObject->velocity += deltaVelocityThisFrame;

			// Position change is based on the velocity over this time frame
			// This part: (Vel * DeltaTime)	
			glm::vec3 deltaPosition = pObject->velocity * (float)deltaTime;

			pObject->mesh->drawPosition.x += deltaPosition.x;
			pObject->mesh->drawPosition.y += deltaPosition.y;
			pObject->mesh->drawPosition.z += deltaPosition.z;
		}

	}

	// See which object is colliding with which object...
	for (PhysicsBody* pObjectA : this->bodies)
	{

		for (PhysicsBody* pObjectB : this->bodies)
		{
			// Are "A" and "B" the same object
			if (pObjectA == pObjectB)
			{
				// Yup, so skip this
				continue;
			}
			// What's the collision? 
			switch (pObjectA->shapeType)
			{
				case PhysicsShapes::SPHERE:
					switch (pObjectB->shapeType)
					{
						case PhysicsShapes::SPHERE:
							// Sphere - Sphere
							this->m_Sphere_Sphere_IntersectionTest(pObjectA, pObjectB);
							break;
						case PhysicsShapes::MESH_OF_TRIANGLES_INDIRECT:
							// Sphere - Mesh triangle (indirect)
							if (this->m_Sphere_TriMeshIndirect_IntersectionTest(pObjectA, pObjectB))
							{
								std::cout << "Hazzah!" << std::endl;
							}
							break;
						case PhysicsShapes::MESH_OF_TRIANGLES_LOCAL_VERTICES:
							// Shpere - Mesh (local vertices)
							break;
					}//switch (pObjectB->shapeType)
					break;
			}

		}
	}
	return;
}

void PhysicsManager::AddMesh(PhysicsBody* physicsBody)
{
	bodies.push_back(physicsBody);
}

std::vector<cAABB*> PhysicsManager::GenerateAABBs(PhysicsBody* body)
{
	std::vector<cAABB*> aabbs;
	
	body->mesh->calcExtents();

	//populate the aabb map with it's id
	glm::vec3 extent = body->mesh->maxExtents_XYZ - body->mesh->minExtents_XYZ;
	glm::vec3 center = (body->mesh->maxExtents_XYZ + body->mesh->minExtents_XYZ) * 0.5f;

	glm::vec3 boxDimensions = extent / glm::vec3(10,10,10);
	for (int x =0; x != 10; x++)
	{
		for (int y = 0; y != 10; y++)
		{
			for (int z = 0; z != 10; z++)
			{
				cAABB* pAABB = new cAABB();

				pAABB->minXYZ.x = body->mesh->minExtents_XYZ.x + x * boxDimensions.x;
				pAABB->minXYZ.y = body->mesh->minExtents_XYZ.y + y * boxDimensions.y;
				pAABB->minXYZ.z = body->mesh->minExtents_XYZ.z + z * boxDimensions.z;

				pAABB->maxXYZ.x = pAABB->minXYZ.x + boxDimensions.x;
				pAABB->maxXYZ.y = pAABB->minXYZ.y + boxDimensions.y;
				pAABB->maxXYZ.z = pAABB->minXYZ.z + boxDimensions.z;

				aabbs.push_back(pAABB);

			/*	glm::vec3 extents = glm::vec3(extent, extent, extent);

				unsigned int AABB_ID = cAABB::static_getLocationIndex(pAABB->minXYZ, extents);

				body->aabbs[AABB_ID] = pAABB;*/

			}
		}
	}

	return aabbs;
	//for (int i = 0; i < theMeshDrawInfo.numberOfVertices; i++)
	//{
	//	sVertex vert = theMeshDrawInfo.pVertices[i];
	//	glm::vec3 vTestVert = glm::vec3(vert.x, vert.y, vert.z);		
	//	glm::vec3 extents = glm::vec3(100, 100, 100);
	//	unsigned int AABB_ID = cAABB::static_getLocationIndex(vTestVert, extents);

	//	body->aabbs[AABB_ID]->vecVerticesInside.push_back(vTestVert);	
	//}

	//draw the aabb
	//for (std::map< unsigned int, cAABB* >::iterator itAABB = body->aabbs.begin();
	//	itAABB != body->aabbs.end(); itAABB++)
	//{
	//	if (!itAABB->second->vecVerticesInside.empty())
	//	{
	//		glm::vec3 extents = glm::vec3(100, 100, 100);
	//		glm::vec3 location = cAABB::static_getLocationFromIndex(itAABB->second->getLocationIndex(), extents);
	//		std::cout << location.x << " " << location.y << " " << location.z
	//			<< " has " << itAABB->second->vecVerticesInside.size()
	//			<< " vertices inside it." << std::endl;
	//	}
	//}

}

glm::vec3 PhysicsManager::ClosestPtPointTriangle(glm::vec3 p, glm::vec3 a, glm::vec3 b, glm::vec3 c)
{
		glm::vec3 ab = b - a;
	glm::vec3 ac = c - a;
	glm::vec3 bc = c - b;

	// Compute parametric position s for projection P' of P on AB,
	// P' = A + s*AB, s = snom/(snom+sdenom)
	float snom = glm::dot(p - a, ab), sdenom = glm::dot(p - b, a - b);

	// Compute parametric position t for projection P' of P on AC,
	// P' = A + t*AC, s = tnom/(tnom+tdenom)
	float tnom = glm::dot(p - a, ac), tdenom = glm::dot(p - c, a - c);

	if (snom <= 0.0f && tnom <= 0.0f) return a; // Vertex region early out

	// Compute parametric position u for projection P' of P on BC,
	// P' = B + u*BC, u = unom/(unom+udenom)
	float unom = glm::dot(p - b, bc), udenom = glm::dot(p - c, b - c);

	if (sdenom <= 0.0f && unom <= 0.0f) return b; // Vertex region early out
	if (tdenom <= 0.0f && udenom <= 0.0f) return c; // Vertex region early out


	// P is outside (or on) AB if the triple scalar product [N PA PB] <= 0
	glm::vec3 n = glm::cross(b - a, c - a);
	float vc = glm::dot(n, glm::cross(a - p, b - p));
	// If P outside AB and within feature region of AB,
	// return projection of P onto AB
	if (vc <= 0.0f && snom >= 0.0f && sdenom >= 0.0f)
		return a + snom / (snom + sdenom) * ab;

	// P is outside (or on) BC if the triple scalar product [N PB PC] <= 0
	float va = glm::dot(n, glm::cross(b - p, c - p));
	// If P outside BC and within feature region of BC,
	// return projection of P onto BC
	if (va <= 0.0f && unom >= 0.0f && udenom >= 0.0f)
		return b + unom / (unom + udenom) * bc;

	// P is outside (or on) CA if the triple scalar product [N PC PA] <= 0
	float vb = glm::dot(n, glm::cross(c - p, a - p));
	// If P outside CA and within feature region of CA,
	// return projection of P onto CA
	if (vb <= 0.0f && tnom >= 0.0f && tdenom >= 0.0f)
		return a + tnom / (tnom + tdenom) * ac;

	// P must project inside face region. Compute Q using barycentric coordinates
	float u = va / (va + vb + vc);
	float v = vb / (va + vb + vc);
	float w = 1.0f - u - v; // = vc / (va + vb + vc)
	return u * a + v * b + w * c;
}

bool PhysicsManager::m_Sphere_Sphere_IntersectionTest(PhysicsBody* pSphereA, PhysicsBody* pShpereB)
{
	// Do we have a mesh manager? 
	if (!this->meshManager)
	{
		return false;
	}

	// Does the physics object have a mesh object associated? 
	if (!pSphereA->mesh || !pShpereB->mesh)
	{
		return false;
	}

	PhysicsShapes::sSphere* sphereA = (PhysicsShapes::sSphere*)(pSphereA->shape);
	PhysicsShapes::sSphere* sphereB = (PhysicsShapes::sSphere*)(pShpereB->shape);


	float distance = glm::distance(pSphereA->mesh->drawPosition, pShpereB->mesh->drawPosition);
	float radiusSum = sphereA->radius + sphereB->radius;

	if (distance < radiusSum)
	{

		// impact speed
		glm::vec3 v = (pSphereA->velocity - pShpereB->velocity);
		glm::vec3 normal = (pSphereA->mesh->drawPosition - pShpereB->mesh->drawPosition);
		
		normal = glm::normalize(normal);

		float vn = glm::dot(v, normal);

		//// sphere intersecting but moving away from each other already
		if (vn > 0.0f)
			return false;

		// collision impulse
		float i = (-(1.0f) * vn) / (pSphereA->inverseMass + pShpereB->inverseMass);
		glm::vec3 impulse = normal * i;

		// change in momentum
		pSphereA->velocity += impulse * pSphereA->inverseMass;
		pShpereB->velocity -= impulse * pShpereB->inverseMass;
		pSphereA->mesh->color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
		pShpereB->mesh->color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);

		return true;
	}

}

bool PhysicsManager::m_Sphere_TriMeshIndirect_IntersectionTest(PhysicsBody* sphere, PhysicsBody* triMesh)
{
	// Do we have a mesh manager? 
	if (!this->meshManager)
	{
		return false;
	}

	// Does the physics object have a mesh object associated? 
	if (!triMesh->mesh)
	{
		return false;
	}

	// Get the info about this shape, specifically
	PhysicsShapes::sMeshOfTriangles_Indirect* pTriangleMesh =
		(PhysicsShapes::sMeshOfTriangles_Indirect*)(triMesh->shape);

	PhysicsShapes::sSphere* pSphere = (PhysicsShapes::sSphere*)(sphere->shape);

	sModelDrawInfo theMeshDrawInfo;

	// Find the raw mesh information from the VAO manager
	if (!this->meshManager->GetModelDrawInfo(pTriangleMesh->meshName, theMeshDrawInfo))
	{
		// Didn't find it
		return false;
	}


	//	glm::vec3 closestPointToTriangle = glm::vec3(FLT_MAX, FLT_MAX, FLT_MAX);
	float closestDistanceSoFar = FLT_MAX;
	glm::vec3 closestTriangleVertices[3] = { glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(0.0f) };
	unsigned int indexOfClosestTriangle = INT_MAX;


	// We now have the mesh object location and the detailed mesh information 
	// Which triangle is closest to this sphere (right now)
	for (unsigned int index = 0; index != theMeshDrawInfo.numberOfIndices; index += 3)
	{
		glm::vec3 verts[3];

		verts[0].x = theMeshDrawInfo.pVertices[theMeshDrawInfo.pIndices[index]].x;
		verts[0].y = theMeshDrawInfo.pVertices[theMeshDrawInfo.pIndices[index]].y;
		verts[0].z = theMeshDrawInfo.pVertices[theMeshDrawInfo.pIndices[index]].z;

		verts[1].x = theMeshDrawInfo.pVertices[theMeshDrawInfo.pIndices[index + 1]].x;
		verts[1].y = theMeshDrawInfo.pVertices[theMeshDrawInfo.pIndices[index + 1]].y;
		verts[1].z = theMeshDrawInfo.pVertices[theMeshDrawInfo.pIndices[index + 1]].z;

		verts[2].x = theMeshDrawInfo.pVertices[theMeshDrawInfo.pIndices[index + 2]].x;
		verts[2].y = theMeshDrawInfo.pVertices[theMeshDrawInfo.pIndices[index + 2]].y;
		verts[2].z = theMeshDrawInfo.pVertices[theMeshDrawInfo.pIndices[index + 2]].z;

		// Transform this object in world space using the same technique we did to render it
		// 
		// Here's the key line: 	
		// 
		//		vertexWorldPos = matModel * vec4( vPos.xyz, 1.0f);
		// 
		// THIS BLOCK OF CODE IS FROM DrawObject()

		glm::mat4 matModel = glm::mat4(1.0f);

		// Translation
		glm::mat4 matTranslate = glm::translate(glm::mat4(1.0f),
			glm::vec3(triMesh->mesh->drawPosition.x,
					triMesh->mesh->drawPosition.y,
					triMesh->mesh->drawPosition.z));

		// Rotation matrix generation
		glm::mat4 matRotation = glm::mat4(triMesh->mesh->get_qOrientation());

		glm::mat4 matScale = glm::scale(glm::mat4(1.0f),
			glm::vec3(triMesh->mesh->drawScale.x,
					triMesh->mesh->drawScale.y,
					triMesh->mesh->drawScale.z));

				// Combine all these transformation
		matModel = matModel * matTranslate;

		matModel = matModel * matRotation;

		matModel = matModel * matScale;

		glm::vec4 vertsWorld[3];
		vertsWorld[0] = (matModel * glm::vec4(verts[0], 1.0f));
		vertsWorld[1] = (matModel * glm::vec4(verts[1], 1.0f));
		vertsWorld[2] = (matModel * glm::vec4(verts[2], 1.0f));

		// And make sure you multiply the normal by the inverse transpose
		// OR recalculate it right here! 

		// ******************************************************

		glm::vec3 thisTriangleClosestPoint = this->ClosestPtPointTriangle(sphere->mesh->drawPosition,vertsWorld[0], vertsWorld[1], vertsWorld[2]);

		// Is this the closest so far
		float distanceToThisTriangle = glm::distance(thisTriangleClosestPoint, sphere->mesh->drawPosition);

		if (distanceToThisTriangle < closestDistanceSoFar)
		{
			// this one is closer
			closestDistanceSoFar = distanceToThisTriangle;
			// Make note of the triangle index
			indexOfClosestTriangle = index;
			// 
			closestTriangleVertices[0] = vertsWorld[0];
			closestTriangleVertices[1] = vertsWorld[1];
			closestTriangleVertices[2] = vertsWorld[2];
		}


	} 

	if (closestDistanceSoFar < pSphere->radius)
	{
		glm::vec3 sphereDirection = sphere->velocity;
		// Normalize... 
		sphereDirection = glm::normalize(sphereDirection);

		// Calcualte the current normal from the TRANSFORMED vertices
		glm::vec3 edgeA = closestTriangleVertices[1] - closestTriangleVertices[0];
		glm::vec3 edgeB = closestTriangleVertices[2] - closestTriangleVertices[0];

		glm::vec3 triNormal = glm::normalize(glm::cross(edgeA, edgeB));
		glm::vec3 reflectionVec = glm::reflect(sphereDirection, triNormal);

		// Update the  velocity based on this reflection vector
		float sphereSpeed = glm::length(sphere->velocity);
		glm::vec3 newVelocity = reflectionVec * sphereSpeed;

		sphere->velocity = newVelocity;

		return true;
	}

	// Didn't hit
	return false;
}

void PhysicsBody::setShape(PhysicsShapes::sSphere* pSphereProps)
{
	this->shape = (void*)pSphereProps;
}

void PhysicsBody::setShape(PhysicsShapes::sPlane* pPlaneProps)
{
	this->shape = (void*)pPlaneProps;
}

void PhysicsBody::setShape(PhysicsShapes::sTriangle* pTriangleProps)
{
	this->shape = (void*)pTriangleProps;
}

void PhysicsBody::setShape(PhysicsShapes::sCapsule * pCapsuleProps)
{
	this->shape = (void*)pCapsuleProps;
}

void PhysicsBody::setShape(PhysicsShapes::sMeshOfTriangles_Indirect * pTriangleMeshProps)
{
	this->shape = (void*)pTriangleMeshProps;
}

void PhysicsBody::setShape(PhysicsShapes::sMeshOfTriangles_LocalVertices * pTriangleMeshProps)
{
	this->shape = (void*)pTriangleMeshProps;
}
