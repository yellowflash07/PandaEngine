#include "cMesh.h"

// Constructor: Called on creation   c'tor
cMesh::cMesh()
{
	this->drawPosition = glm::vec3(0.0f, 0.0f, 0.0f);
//	this->drawOrientation = glm::vec3(0.0f, 0.0f, 0.0f);
	this->setRotationFromEuler(glm::vec3(0.0f, 0.0f, 0.0f));
	this->drawScale = glm::vec3(1.0f);

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

// static
unsigned int cMesh::m_nextUniqueID = cMesh::FIRST_UNIQUE_ID;


// Destructor: Called on deletion   d'tor
cMesh::~cMesh()
{

}

void cMesh::setUniformDrawScale(float scale)
{
	this->drawScale.x = this->drawScale.y = this->drawScale.z = scale;
	return;
}


