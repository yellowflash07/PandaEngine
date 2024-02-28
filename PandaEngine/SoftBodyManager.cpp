#include "SoftBodyManager.h"

SoftBodyManager::SoftBodyManager(MeshManager* meshManager)
{
	this->meshManager = meshManager;
}

SoftBodyManager::~SoftBodyManager()
{
}

void SoftBodyManager::Update(double deltaTime)
{
	for (SoftBody* pSoftBody : vecSoftBodies)
	{
		pSoftBody->VerletUpdate(deltaTime);
		pSoftBody->UpdateVertexPositions();
		meshManager->UpdateVAOBuffers(pSoftBody->meshName, pSoftBody->ModelInfo);
		pSoftBody->UpdateNormals();
	}
}

SoftBody* SoftBodyManager::CreateSoftBody(std::string friendlyName)
{
	SoftBody* pSoftBody = new SoftBody();	
	//if (!meshManager->GetTransformedMeshDrawInfo(friendlyName, pSoftBody->ModelInfo))
	if (!meshManager->GetModelDrawInfo(friendlyName, pSoftBody->ModelInfo))
	{
		delete pSoftBody;
		return NULL;
	}
	pSoftBody->meshName = friendlyName;
	cMesh* pMesh = meshManager->FindMeshByFriendlyName(friendlyName);
	pSoftBody->pMesh = pMesh;
	pSoftBody->Init();
	vecSoftBodies.push_back(pSoftBody);
	return pSoftBody;
}
