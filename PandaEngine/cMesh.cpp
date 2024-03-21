#include "cMesh.h"
#include <iostream>
#include "../cVAOManager/cVAOManager.h"
#include "../Basic Shader Manager/cShaderManager.h"
// Constructor: Called on creation   c'tor
cMesh::cMesh()
{

	this->bIsWireframe = false;
	this->bDoNotLight = false;

	this->bIsVisible = true;

	this->bUseDebugColours = false;
	this->wholeObjectDebugColourRGBA = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	this->color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	// Set uniqueID
	this->m_UniqueID = cMesh::m_nextUniqueID;
	cMesh::m_nextUniqueID++;
}

unsigned int cMesh::getUniqueID(void)
{
	return this->m_UniqueID;
}

void cMesh::calcExtents(void)
{
	glm::mat4 matModel = transform.GetTransform();

	for (size_t i = 0; i < modelDrawInfo.numberOfVertices; i++)
	{
		glm::vec3 vert = glm::vec3(modelDrawInfo.pVertices[i].x,
									modelDrawInfo.pVertices[i].y,
									modelDrawInfo.pVertices[i].z);
		vert =  (matModel * glm::vec4(vert, 1.0f));

		modelDrawInfo.pVertices[i].x = vert.x;
		modelDrawInfo.pVertices[i].y = vert.y;
		modelDrawInfo.pVertices[i].z = vert.z;
	}

	for (size_t i = 0; i < modelDrawInfo.numberOfTriangles; i++)
	{

		glm::vec3 v1 = modelDrawInfo.pTriangles[i].v1;
		glm::vec3 v2 = modelDrawInfo.pTriangles[i].v2;
		glm::vec3 v3 = modelDrawInfo.pTriangles[i].v3;
		modelDrawInfo.pTriangles[i].v1 = (matModel * glm::vec4(v1, 1.0f));
		modelDrawInfo.pTriangles[i].v2 = (matModel * glm::vec4(v2, 1.0f));
		modelDrawInfo.pTriangles[i].v3 = (matModel * glm::vec4(v3, 1.0f));
	}

	modelDrawInfo.calcExtents();

	maxExtents_XYZ = modelDrawInfo.maxExtents_XYZ;
	minExtents_XYZ = modelDrawInfo.minExtents_XYZ;
}

void cMesh::AddChild(cMesh* child)
{
	child->isChild = true;
	this->vec_pChildMeshes.push_back(child);
}


// static
unsigned int cMesh::m_nextUniqueID = cMesh::FIRST_UNIQUE_ID;


cMesh::cMesh(std::string meshName, std::string friendlyName)
{
	//std::cout << "!!!!!!!!!!!!!!" << std::endl;
	std::cout << meshName << "," << friendlyName << std::endl;
	cVAOManager* pVAOManager = cVAOManager::getInstance();	
	//HACK:
	cShaderManager* pShaderManager = cShaderManager::getInstance();
	GLint shaderID = pShaderManager->getIDFromFriendlyName("shader01");
	if (!pVAOManager->LoadModelIntoVAOAI(meshName, this->modelDrawInfo, shaderID))
	{
	//	std::cout << "Didn't load model" << std::endl;
	}
	else
	{
	//	std::cout << "Loaded model" << std::endl;
	}
	this->meshName = meshName;
	this->uniqueName = modelDrawInfo.uniqueName;
	this->friendlyName = friendlyName;
	this->bIsWireframe = false;
	this->bDoNotLight = false;
	this->bIsVisible = true;
	this->bUseDebugColours = false;
	this->wholeObjectDebugColourRGBA = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	this->color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	this->isSkyBox = false;
	this->hasVertexColors = false;
	this->isReflective = false;
	this->texture[0] = "";
	this->texture[1] = "";
	this->texture[2] = "";
	this->texture[3] = "";
	this->textureRatio[0] = 1.0f;
	this->textureRatio[1] = 1.0f;
	this->textureRatio[2] = 1.0f;
	this->textureRatio[3] = 1.0f;
	this->m_UniqueID = cMesh::m_nextUniqueID;
	cMesh::m_nextUniqueID++;
}

// Destructor: Called on deletion   d'tor
cMesh::~cMesh()
{

}




