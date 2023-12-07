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

void cMesh::calcExtents(void)
{
	glm::mat4 matModel = glm::mat4(1.0f);

	// Translation
	glm::mat4 matTranslate = glm::translate(glm::mat4(1.0f),
		glm::vec3(drawPosition.x,
			     drawPosition.y,
			     drawPosition.z));

	// Rotation matrix generation
	glm::mat4 matRotation = glm::mat4(get_qOrientation());

	glm::mat4 matScale = glm::scale(glm::mat4(1.0f),
									glm::vec3(drawScale.x,drawScale.y,drawScale.z));

	// Combine all these transformation
	matModel = matModel * matTranslate;

	matModel = matModel * matRotation;

	matModel = matModel * matScale;

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
	modelDrawInfo.calcExtents();

	maxExtents_XYZ = modelDrawInfo.maxExtents_XYZ;
	minExtents_XYZ = modelDrawInfo.minExtents_XYZ;
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


