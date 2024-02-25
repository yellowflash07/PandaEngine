#pragma once

#include <vector>
#include "../cVAOManager/sModelDrawInfo.h"
#include "cMesh.h"
#include "PhysicsManager.h"
class SoftBody
{
public:
	SoftBody();
	~SoftBody();

	void Init();
	void UpdateVertexPositions(void);
	void CreateRandomBracing(unsigned int numberOfBraces, float minDistanceBetweenVertices);
	void VerletUpdate(double deltaTime);
	void UpdateNormals(void);
	struct sParticle
	{
		glm::vec3 position = glm::vec3(0.0f);
		glm::vec3 old_position = glm::vec3(0.0f);
		// Pointer back to the model vertex info
		sVertex* pModelVertex = NULL;
	};

	struct sConstraint
	{
		sConstraint() {}
		sParticle* pParticleA = NULL;
		sParticle* pParticleB = NULL;
		// How far away they are supposed to be 'at rest'
		float restLength = 0.0f;

		unsigned int numIterations = 1;

		// if false, this isnt' checked
		// Like if the constraint is 'broken'
		bool bIsActive = true;
		// This is interesting, too
		// From: I Spent a Week Making an AI's Video Game Idea - YouTube
		// https://www.youtube.com/watch?v=PGk0rnyTa1U&ab_channel=SebastianLague
		// He's using it as to fix the end of the constraint
		bool bIsLocked = false;
	};

	void cleanZeros(glm::vec3& value);


	float calcDistanceBetween(sParticle* pPartA, sParticle* pPartB);

	void AddCollisionSphere(PhysicsBody* pSphere);

	glm::vec3 acceleration = glm::vec3(0.0f);
	sModelDrawInfo ModelInfo;
	std::vector< sParticle* > vec_pParticles;
	cMesh* pMesh;
	std::string meshName;
	glm::vec3 scale;
	glm::quat rotation;
	glm::vec3 position;
	glm::mat4 matModel;
private:
	void ApplyCollision();
	void SatisfyConstraints(void);
	std::vector< sConstraint* > vec_pConstraints;
	std::vector<PhysicsBody*> vec_pSpheres;
	glm::mat4 matInitialTransform;
};

