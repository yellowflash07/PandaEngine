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
	
		if (pSoftBody->pMesh != NULL)
		{
			pSoftBody->UpdateVertexPositions();
			meshManager->UpdateVAOBuffers(pSoftBody->pMesh->uniqueName, pSoftBody->ModelInfo);
			pSoftBody->UpdateNormals();
		}
		
	}
}

SoftBody* SoftBodyManager::CreateSoftBody(std::string friendlyName)
{
	SoftBody* pSoftBody = new SoftBody();

	if (!meshManager->GetModelDrawInfo(friendlyName, pSoftBody->ModelInfo))
	{
		delete pSoftBody;
		return NULL;
	}
	cMesh* pMesh = meshManager->FindMeshByFriendlyName(friendlyName);
	pSoftBody->pMesh = pMesh;
	pSoftBody->Init();
	vecSoftBodies.push_back(pSoftBody);
	return pSoftBody;
}

void SoftBodyManager::AddSoftBody(SoftBody* pSoftBody)
{
	vecSoftBodies.push_back(pSoftBody);
}
