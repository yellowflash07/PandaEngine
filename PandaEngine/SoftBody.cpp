#include "SoftBody.h"

SoftBody::SoftBody()
{
}

SoftBody::~SoftBody()
{
}

void SoftBody::Init()
{
	ogVertices = new sVertex[this->ModelInfo.numberOfVertices];

	for (unsigned int index = 0; index != this->ModelInfo.numberOfVertices; index++)
	{
		sParticle* pParticle = new sParticle();

		glm::vec4 vert = glm::vec4(this->ModelInfo.pVertices[index].x,
									this->ModelInfo.pVertices[index].y,
									this->ModelInfo.pVertices[index].z, 1.0f);

		ogVertices[index] = this->ModelInfo.pVertices[index];

		// Apply transformation
		vert = pMesh->GetTransform() * vert;

		pParticle->position = glm::vec3(vert.x, vert.y, vert.z);

		pParticle->old_position = pParticle->position;
	
		pParticle->pModelVertex = &(this->ModelInfo.pVertices[index]);

		this->vec_pParticles.push_back(pParticle);
	}

	// Constraints
	for (unsigned int index = 0; index < this->ModelInfo.numberOfIndices; index += 3)
	{
		sParticle* pParticle1 = this->vec_pParticles[this->ModelInfo.pIndices[index /* +0 */]];
		sParticle* pParticle2 = this->vec_pParticles[this->ModelInfo.pIndices[index + 1]];
		sParticle* pParticle3 = this->vec_pParticles[this->ModelInfo.pIndices[index + 2]];

		sConstraint* pEdge1 = new sConstraint();
		pEdge1->pParticleA = pParticle1;
		pEdge1->pParticleB = pParticle2;
		pEdge1->restLength = this->calcDistanceBetween(pEdge1->pParticleA, pEdge1->pParticleB);

		sConstraint* pEdge2 = new sConstraint();
		pEdge2->pParticleA = pParticle2;
		pEdge2->pParticleB = pParticle3;
		pEdge2->restLength = this->calcDistanceBetween(pEdge2->pParticleA, pEdge2->pParticleB);

		sConstraint* pEdge3 = new sConstraint();
		pEdge3->pParticleA = pParticle3;
		pEdge3->pParticleB = pParticle1;
		pEdge3->restLength = this->calcDistanceBetween(pEdge3->pParticleA, pEdge3->pParticleB);

		this->vec_pConstraints.push_back(pEdge1);
		this->vec_pConstraints.push_back(pEdge2);
		this->vec_pConstraints.push_back(pEdge3);
	}
}

void SoftBody::UpdateVertexPositions(void)
{
	for (size_t i = 0; i < this->ModelInfo.numberOfVertices; i++)
	{
		sParticle* curParticle = this->vec_pParticles[i];

		if (curParticle->bDisconnected)
		{
			continue;
		}

		glm::vec4 vert = glm::vec4(curParticle->position, 1.0f);
		
		glm::mat4 matModel = pMesh->GetTransform();
		matModel = glm::inverse((matModel));

		vert = matModel * vert;
		this->ModelInfo.pVertices[i].x = vert.x;
		this->ModelInfo.pVertices[i].y = vert.y;
		this->ModelInfo.pVertices[i].z = vert.z;
	}
}

void SoftBody::CreateRandomBracing(unsigned int numberOfBraces, float minDistanceBetweenVertices)
{

	for (unsigned int count = 0; count != numberOfBraces; count++)
	{
		// Assume the distance is OK
		bool bKeepLookingForParticles = false;

		do
		{
			// Assume the distance is OK
			bKeepLookingForParticles = false;
			unsigned int particleIndex1 = rand() % this->vec_pParticles.size();
			unsigned int particleIndex2 = rand() % this->vec_pParticles.size();

			sParticle* pParticle1 = this->vec_pParticles[particleIndex1];
			sParticle* pParticle2 = this->vec_pParticles[particleIndex2];
			float distBetween = this->calcDistanceBetween(pParticle1, pParticle2);

			// Distance OK?
			if (distBetween < minDistanceBetweenVertices)
			{
				// No
				bKeepLookingForParticles = true;
			}
			else
			{
				// Distance is OK, so make a constraint
				sConstraint* pBracingConstraint = new sConstraint();
				pBracingConstraint->pParticleA = pParticle1;
				pBracingConstraint->pParticleB = pParticle2;
				pBracingConstraint->restLength = this->calcDistanceBetween(pBracingConstraint->pParticleA, pBracingConstraint->pParticleB);

				this->vec_pConstraints.push_back(pBracingConstraint);
			}

		} while (bKeepLookingForParticles);

	}

	return;
}

void SoftBody::VerletUpdate(double deltaTime)
{	

	for (sParticle* pCurrentParticle : vec_pParticles)
	{
		if (pCurrentParticle->bIsLocked)
		{
			continue;
		}

		glm::vec3 current_pos = pCurrentParticle->position;
		glm::vec3 old_pos = pCurrentParticle->old_position;

		// This is the actual Verlet integration step (notice there isn't a velocity)
		pCurrentParticle->position += (current_pos - old_pos) + (this->acceleration * (float)(deltaTime * deltaTime));

		pCurrentParticle->old_position = current_pos;

		// Check if there is a LARGE different between old and new positions
		this->cleanZeros(pCurrentParticle->position);
		this->cleanZeros(pCurrentParticle->old_position);
	}

	ApplyCollision();
	SatisfyConstraints();
}

void SoftBody::UpdateNormals(void)
{

	// Go through each triagle and calculate the normals. 
	// Accumulate those normals at each vertex.
	// Normalize the normals

	// Clear the existing normals because they are invalid
	for (unsigned int vertIndex = 0; vertIndex != this->ModelInfo.numberOfVertices; vertIndex++)
	{
		this->ModelInfo.pVertices[vertIndex].nx = 0.0f;
		this->ModelInfo.pVertices[vertIndex].ny = 0.0f;
		this->ModelInfo.pVertices[vertIndex].nz = 0.0f;
	}


	for (unsigned int index = 0; index != this->ModelInfo.numberOfIndices; index+=3)
	{
		// Indices are sets of 3, one per 
		unsigned int vertAIndex = this->ModelInfo.pIndices[index + 0];
		unsigned int vertBIndex = this->ModelInfo.pIndices[index + 1];
		unsigned int vertCIndex = this->ModelInfo.pIndices[index + 2];

		//glm::mat4 matModel = pMesh->GetTransform();
		//matModel = glm::inverse(matModel);

		// note the references so that when we update this, it will update the mesh
		// (otherwise we'll be updating a copy of it)
		sVertex vertexA = this->ModelInfo.pVertices[vertAIndex];
		sVertex vertexB = this->ModelInfo.pVertices[vertBIndex];
		sVertex vertexC = this->ModelInfo.pVertices[vertCIndex];

		glm::vec3 vertA = glm::vec3(vertexA.x, vertexA.y, vertexA.z);
		//vertA = glm::vec3(matModel * glm::vec4(vertA, 1.0f));
		glm::vec3 vertB = glm::vec3(vertexB.x, vertexB.y, vertexB.z);
		//vertB = glm::vec3(matModel * glm::vec4(vertB, 1.0f));
		glm::vec3 vertC = glm::vec3(vertexC.x, vertexC.y, vertexC.z);
		//vertC = glm::vec3(matModel * glm::vec4(vertC, 1.0f));

		glm::vec3 triangleEdgeAtoB = vertB - vertA;
		glm::vec3 triangleEdgeAtoC = vertC - vertA;

		glm::vec3 theNormal = glm::cross(triangleEdgeAtoB, triangleEdgeAtoC);
		theNormal = glm::normalize(theNormal);

		// Add (accumulate) this normal to the three vertices
		vertexA.nx += theNormal.x;
		vertexA.ny += theNormal.y;
		vertexA.nz += theNormal.z;

		vertexB.nx += theNormal.x;
		vertexB.ny += theNormal.y;
		vertexB.nz += theNormal.z;

		vertexC.nx += theNormal.x;
		vertexC.ny += theNormal.y;
		vertexC.nz += theNormal.z;

		// Update the vertices
		this->ModelInfo.pVertices[vertAIndex] = vertexA;
		this->ModelInfo.pVertices[vertBIndex] = vertexB;
		this->ModelInfo.pVertices[vertCIndex] = vertexC;
	}// for ( unsigned int triIndex = 0

	// Now normalize the accumulated normals
	for (unsigned int vertIndex = 0; vertIndex != this->ModelInfo.numberOfVertices; vertIndex++)
	{
		glm::vec3 theNomral = glm::vec3(this->ModelInfo.pVertices[vertIndex].nx,
										this->ModelInfo.pVertices[vertIndex].ny,
										this->ModelInfo.pVertices[vertIndex].nz);

		theNomral = glm::normalize(theNomral);

		this->ModelInfo.pVertices[vertIndex].nx = theNomral.x;
		this->ModelInfo.pVertices[vertIndex].ny = theNomral.y;
		this->ModelInfo.pVertices[vertIndex].nz = theNomral.z;
	}

	return;
}

void SoftBody::cleanZeros(glm::vec3& value)
{
	if (fabs(value.x) < 0.0001f)
	{
		value.x = 0.0f;
	}
	if (fabs(value.y) < 0.0001f)
	{
		value.y = 0.0f;
	}
	if (fabs(value.z) < 0.0001f)
	{
		value.z = 0.0f;
	}
}

float SoftBody::calcDistanceBetween(sParticle* pPartA, sParticle* pPartB)
{
	return glm::distance(pPartA->position, pPartB->position);
}

void SoftBody::AddCollisionSphere(PhysicsBody* pSphere)
{
	this->vec_pSpheres.push_back(pSphere);
}

bool SoftBody::LockParticle(unsigned int index, bool bLock)
{
	if (vec_pParticles.size() > index)
	{
		vec_pParticles[index]->bIsLocked = bLock;
		return true;
	}

	return false;
}

void SoftBody::DisconnectParticle(unsigned int index)
{
	if (vec_pParticles.size() > index)
	{
		sParticle* pParticle = vec_pParticles[index];
		pParticle->bDisconnected = true;

		// Deactivate constraints that have this particle in them
		for (sConstraint* pCurConstraint : this->vec_pConstraints)
		{
			if (pCurConstraint->pParticleA == pParticle || pCurConstraint->pParticleB == pParticle)
			{
				pCurConstraint->bIsActive = false;
			}
		}		

		//update the model info to reflect the change
		//assinging nan to the vertex will make it invisible/dead
		this->ModelInfo.pVertices[index].x = sqrt(-1);
		this->ModelInfo.pVertices[index].y = sqrt(-1);
		this->ModelInfo.pVertices[index].z = sqrt(-1);

	}
}

void SoftBody::Reset()
{
	// Reset the particles to their original positions
	for (size_t i = 0; i < vec_pParticles.size(); i++)
	{
		glm::vec4 vert = glm::vec4(this->ogVertices[i].x,
									this->ogVertices[i].y,
									this->ogVertices[i].z, 1.0f);
		vert = pMesh->GetTransform() * vert;
		vec_pParticles[i]->position = vert;
		vec_pParticles[i]->old_position = vert;
		vec_pParticles[i]->bDisconnected = false;
		vec_pParticles[i]->bIsLocked = false;
	}

	for (size_t i = 0; i < vec_pConstraints.size(); i++)
	{
		vec_pConstraints[i]->bIsActive = true;
	}

}

void SoftBody::ApplyCollision()
{	
	//for (sParticle* pCurrentParticle : vec_pParticles)
	for (int i = 0;i < vec_pParticles.size(); i++)
	{
		sParticle* pCurrentParticle = vec_pParticles[i];
		// Check for collision with the spheres
		for (PhysicsBody* sphereBody : vec_pSpheres)
		{
			PhysicsShapes::sSphere* sphere = (PhysicsShapes::sSphere*)(sphereBody->shape);

			glm::vec3 sphereCentre = sphereBody->mesh->drawPosition;
			float sphereRadius = sphere->radius;

			float distanceToSphere = glm::distance(pCurrentParticle->position,
													sphereCentre);
			if (distanceToSphere < sphereRadius)
			{
				DisconnectParticle(i);
				// it's 'inside' the sphere
				// Shift or slide the point along the ray from the centre of the sphere
				//glm::vec3 particleToCentreRay = pCurrentParticle->position - sphereCentre;
				// Normalize to get the direction
				//particleToCentreRay = glm::normalize(particleToCentreRay);
				//pCurrentParticle->position = (particleToCentreRay * sphereRadius) + sphereCentre;
			}
		}
	}

}
void SoftBody::SatisfyConstraints(void)
{
	const unsigned int NUM_ITERATIONS = 5;

	for (unsigned int iteration = 0; iteration != NUM_ITERATIONS; iteration++)
	{
		// This is ONE pass of the constraint resolution
		for (sConstraint* pCurConstraint : this->vec_pConstraints)
		{
			if (pCurConstraint->bIsActive)
			{
				SoftBody::sParticle* pX1 = pCurConstraint->pParticleA;
				SoftBody::sParticle* pX2 = pCurConstraint->pParticleB;

				if (pX1->bDisconnected && pX2->bDisconnected)
				{
					pCurConstraint->bIsActive = false;
					continue;
				}

				glm::vec3 delta = pX2->position - pX1->position;

				float deltaLength = glm::length(delta);


				float diff = (deltaLength - pCurConstraint->restLength) / deltaLength;

				// If we were having this 'tear' or break apart, 
				//	you could check some maximum length and if it's 'too long'
				//	then the constraint 'breaks'
				// Handle this by:
				// - Setting a bool (where it doesn't check the constraint any more)
				// - Remove the constraint (but removing from a vector is sketchy...)

//				if ( diff > 0.1f )
//				{
//					pCurConstraint->bIsActive = false;
//				}

				// Making this non-one, will change how quickly the objects move together
				// For example, making this < 1.0 will make it "bouncier"
				float tightnessFactor = 1.0f;

				if (!pX1->bIsLocked)
				{
					pX1->position += delta * 0.5f * diff * tightnessFactor;

				}
				if (!pX2->bIsLocked)
				{
					pX2->position -= delta * 0.5f * diff * tightnessFactor;
				}
				//pX1->position += delta * 0.5f * diff * tightnessFactor;
				//pX2->position -= delta * 0.5f * diff * tightnessFactor;

				this->cleanZeros(pX1->position);
				this->cleanZeros(pX2->position);
			}//if (pCurConstraint->bIsActive)

		}//for (sConstraint* pCurConstraint...
	}//for ( unsigned int iteration

	return;
}



