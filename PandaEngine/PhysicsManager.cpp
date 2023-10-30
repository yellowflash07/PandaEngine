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
#pragma region  OLD
	//for (size_t i = 0; i < bodies.size(); i++)
	//{
	//	PhysicsBody* pb = bodies[i];

	//	glm::vec3 deltaVelocityThisFrame = pb->acceleration * (float)deltaTime;

	//	// Update the velocity based on this delta velocity
	//	// Then this part: NewVelocity = LastVel + ...
	//	pb->velocity += deltaVelocityThisFrame;


	//	// Position change is based on the velocity over this time frame
	//	// This part: (Vel * DeltaTime)	
	//	glm::vec3 deltaPosition = pb->velocity * (float)deltaTime;

	//	// ...then this part: NewPosition = LastPos + ...
	//	// Upatate the position based on this delta position
	//	pb->mesh->drawPosition.x += deltaPosition.x;
	//	pb->mesh->drawPosition.y += deltaPosition.y;
	//	pb->mesh->drawPosition.z += deltaPosition.z;

	//	if (pb->mesh->friendlyName == "Sphere")
	//	{

	//		glm::vec3 closestPointToTriangle = glm::vec3(FLT_MAX, FLT_MAX, FLT_MAX);
	//		float closestDistanceSoFar = FLT_MAX;
	//		glm::vec3 closestTriangleVertices[3] = { glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(0.0f) };
	//		unsigned int indexOfClosestTriangle = INT_MAX;
	//		sModelDrawInfo groundMeshInfo;
	//		
	//		cMesh* groundMesh = meshManager->FindMeshByFriendlyName("Ground");
	//		float sphereRadius = 1.0f;

	//		if (meshManager->GetModelDrawInfo("Ground", groundMeshInfo))
	//		{
	//			// Which triangle is closest to this sphere (right now)
	//			for (unsigned int index = 0; index != groundMeshInfo.numberOfIndices; index += 3)
	//			{
	//				glm::vec3 verts[3];

	//				verts[0].x = groundMeshInfo.pVertices[groundMeshInfo.pIndices[index]].x;
	//				verts[0].y = groundMeshInfo.pVertices[groundMeshInfo.pIndices[index]].y;
	//				verts[0].z = groundMeshInfo.pVertices[groundMeshInfo.pIndices[index]].z;

	//				verts[1].x = groundMeshInfo.pVertices[groundMeshInfo.pIndices[index + 1]].x;
	//				verts[1].y = groundMeshInfo.pVertices[groundMeshInfo.pIndices[index + 1]].y;
	//				verts[1].z = groundMeshInfo.pVertices[groundMeshInfo.pIndices[index + 1]].z;

	//				verts[2].x = groundMeshInfo.pVertices[groundMeshInfo.pIndices[index + 2]].x;
	//				verts[2].y = groundMeshInfo.pVertices[groundMeshInfo.pIndices[index + 2]].y;
	//				verts[2].z = groundMeshInfo.pVertices[groundMeshInfo.pIndices[index + 2]].z;

	//				glm::mat4 matModel = glm::mat4(1.0f);

	//				// Translation
	//				glm::mat4 matTranslate = glm::translate(glm::mat4(1.0f),
	//					glm::vec3(groundMesh->drawPosition.x,
	//						groundMesh->drawPosition.y,
	//						groundMesh->drawPosition.z));

	//				// Rotation matrix generation
	//				glm::mat4 matRotateX = glm::rotate(glm::mat4(1.0f),
	//					groundMesh->eulerRotation.x, // (float)glfwGetTime(),
	//					glm::vec3(1.0f, 0.0, 0.0f));


	//				glm::mat4 matRotateY = glm::rotate(glm::mat4(1.0f),
	//					groundMesh->eulerRotation.y, // (float)glfwGetTime(),
	//					glm::vec3(0.0f, 1.0, 0.0f));

	//				glm::mat4 matRotateZ = glm::rotate(glm::mat4(1.0f),
	//					groundMesh->eulerRotation.z, // (float)glfwGetTime(),
	//					glm::vec3(0.0f, 0.0, 1.0f));

	//				// Scaling matrix
	//				glm::mat4 matScale = glm::scale(glm::mat4(1.0f),
	//												glm::vec3(1,1,1));
	//				//--------------------------------------------------------------

	//				// Combine all these transformation
	//				matModel = matModel * matTranslate;

	//				matModel = matModel * matRotateX;
	//				matModel = matModel * matRotateY;
	//				matModel = matModel * matRotateZ;

	//				matModel = matModel * matScale;


	//				// vertexWorldPos = matModel * vec4(vPos.xyz, 1.0f);

	//				glm::vec4 vertsWorld[3];
	//				vertsWorld[0] = (matModel * glm::vec4(verts[0], 1.0f));
	//				vertsWorld[1] = (matModel * glm::vec4(verts[1], 1.0f));
	//				vertsWorld[2] = (matModel * glm::vec4(verts[2], 1.0f));

	//				// And make sure you multiply the normal by the inverse transpose
	//				// OR recalculate it right here! 

	//				// ******************************************************

	//				glm::vec3 thisTriangleClosestPoint = ClosestPtPointTriangle(pb->mesh->drawPosition,
	//					vertsWorld[0], vertsWorld[1], vertsWorld[2]);

	//				// Is this the closest so far
	//				float distanceToThisTriangle = glm::distance(thisTriangleClosestPoint, pb->mesh->drawPosition);

	//				if (distanceToThisTriangle < closestDistanceSoFar)
	//				{
	//					// this one is closer
	//					closestDistanceSoFar = distanceToThisTriangle;
	//					// Make note of the triangle index
	//					indexOfClosestTriangle = index;
	//					// 
	//					closestTriangleVertices[0] = vertsWorld[0];
	//					closestTriangleVertices[1] = vertsWorld[1];
	//					closestTriangleVertices[2] = vertsWorld[2];
	//				}
	//			}

	//			std::cout << closestDistanceSoFar << std::endl;
	//			if (closestDistanceSoFar < sphereRadius)
	//			{
	//				// Hit it!
	//				// Take the normal of that triangle and bounce the sphere along it
	//				// Calculate the current "direction" vector 
	//				// We're using the velocity
	//				glm::vec3 sphereDirection = pb->velocity;
	//				// Normalize... 
	//				sphereDirection = glm::normalize(sphereDirection);

	//				// Calcualte the current normal from the TRANSFORMED vertices
	//				glm::vec3 edgeA = closestTriangleVertices[1] - closestTriangleVertices[0];
	//				glm::vec3 edgeB = closestTriangleVertices[2] - closestTriangleVertices[0];

	//				glm::vec3 triNormal = glm::normalize(glm::cross(edgeA, edgeB));

	//				// Calculate the reflection vector from the normal	
	//				// https://registry.khronos.org/OpenGL-Refpages/gl4/html/reflect.xhtml
	//				// 1st parameter is the "incident" vector
	//				// 2nd parameter is the "normal" vector
	//				glm::vec3 reflectionVec = glm::reflect(sphereDirection, triNormal);

	//				// Update the  velocity based on this reflection vector
	//				float sphereSpeed = glm::length(pb->velocity);
	//				glm::vec3 newVelocity = reflectionVec * sphereSpeed;

	//				pb->velocity = newVelocity;
	//			}

	//		}
	//	}

	//}
#pragma endregion
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


	//for (PhysicsBody* pObject : this->bodies)
	//{
	//	if (pObject->mesh)
	//	{
	//		pObject->pTheAssociatedMesh->setDrawPosition(pObject->position);
	//		pObject->pTheAssociatedMesh->setDrawOrientation(pObject->orientation);
	//	}
	//}

	return;
}

void PhysicsManager::AddMesh(PhysicsBody* physicsBody)
{
	bodies.push_back(physicsBody);
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
		glm::mat4 matRotateX = glm::rotate(glm::mat4(1.0f),
			triMesh->mesh->eulerRotation.x,
			glm::vec3(1.0f, 0.0, 0.0f));


		glm::mat4 matRotateY = glm::rotate(glm::mat4(1.0f),
			triMesh->mesh->eulerRotation.y,
			glm::vec3(0.0f, 1.0, 0.0f));

		glm::mat4 matRotateZ = glm::rotate(glm::mat4(1.0f),
			triMesh->mesh->eulerRotation.z,
			glm::vec3(0.0f, 0.0, 1.0f));

				// Combine all these transformation
		matModel = matModel * matTranslate;

		matModel = matModel * matRotateX;
		matModel = matModel * matRotateY;
		matModel = matModel * matRotateZ;

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
