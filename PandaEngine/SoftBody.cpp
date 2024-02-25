#include "SoftBody.h"

SoftBody::SoftBody()
{
}

SoftBody::~SoftBody()
{
}

void SoftBody::Init()
{
	for (unsigned int index = 0; index != this->ModelInfo.numberOfVertices; index++)
	{
		sParticle* pParticle = new sParticle();

		glm::vec4 vert = glm::vec4(this->ModelInfo.pVertices[index].x,
						this->ModelInfo.pVertices[index].y,
						this->ModelInfo.pVertices[index].z, 1.0f);

		// Apply transformation
		vert = pMesh->GetTransform() * vert;

		pParticle->position = glm::vec3(vert.x, vert.y, vert.z);

		pParticle->old_position = pParticle->position;
	
		pParticle->pModelVertex = &(this->ModelInfo.pVertices[index]);

		this->vec_pParticles.push_back(pParticle);
	}//for ( unsigned int index =...

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
}

void SoftBody::VerletUpdate(double deltaTime)
{	

	for (sParticle* pCurrentParticle : vec_pParticles)
	{
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


	for (unsigned int triIndex = 0; triIndex != this->ModelInfo.numberOfTriangles; triIndex++)
	{
		// Indices are sets of 3, one per 
		unsigned int vertAIndex = this->ModelInfo.pIndices[triIndex + 0];
		unsigned int vertBIndex = this->ModelInfo.pIndices[triIndex + 1];
		unsigned int vertCIndex = this->ModelInfo.pIndices[triIndex + 2];

		//glm::mat4 matModel = pMesh->GetTransform();
		//matModel = glm::inverse(matModel);

		// note the references so that when we update this, it will update the mesh
		// (otherwise we'll be updating a copy of it)
		sVertex& vertexA = this->ModelInfo.pVertices[vertAIndex];
		sVertex& vertexB = this->ModelInfo.pVertices[vertBIndex];
		sVertex& vertexC = this->ModelInfo.pVertices[vertCIndex];

		glm::vec3 vertA = glm::vec3(vertexA.x, vertexA.y, vertexA.z);
	//	vertA = glm::vec3(matModel * glm::vec4(vertA, 1.0f));
		glm::vec3 vertB = glm::vec3(vertexB.x, vertexB.y, vertexB.z);
	//	vertB = glm::vec3(matModel * glm::vec4(vertB, 1.0f));
		glm::vec3 vertC = glm::vec3(vertexC.x, vertexC.y, vertexC.z);
	//	vertC = glm::vec3(matModel * glm::vec4(vertC, 1.0f));

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

void SoftBody::ApplyCollision()
{	
	for (sParticle* pCurrentParticle : vec_pParticles)
	{
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
				// it's 'inside' the sphere
				// Shift or slide the point along the ray from the centre of the sphere
				glm::vec3 particleToCentreRay = pCurrentParticle->position - sphereCentre;
				// Normalize to get the direction
				particleToCentreRay = glm::normalize(particleToCentreRay);
				pCurrentParticle->position = (particleToCentreRay * sphereRadius) + sphereCentre;
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

				pX1->position += delta * 0.5f * diff * tightnessFactor;
				pX2->position -= delta * 0.5f * diff * tightnessFactor;

				this->cleanZeros(pX1->position);
				this->cleanZeros(pX2->position);
			}//if (pCurConstraint->bIsActive)

		}//for (sConstraint* pCurConstraint...
	}//for ( unsigned int iteration

	return;
}



