#ifndef _cMesh_HG_
#define _cMesh_HG_

#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>
#include <string>
#include <vector>


class cMesh
{
public:
	cMesh();		// Called on creation   c'tor
	~cMesh();		// Called on deletion   d'tor

	std::string meshName;

	std::string friendlyName;		// "Ground"

	glm::vec3 drawPosition;

//	glm::vec3 drawOrientation;

	void setRotationFromEuler(glm::vec3 newEulerAngleXYZ)
	{
		this->m_qOrientation = glm::quat(newEulerAngleXYZ);
	}

	void adjustRoationAngleFromEuler(glm::vec3 EulerAngleXYZ_Adjust)
	{
		// To combine quaternion values, you multiply them together
		// Make a quaternion that represents that CHANGE in angle
		glm::quat qChange = glm::quat(EulerAngleXYZ_Adjust);

		// Multiply them together to get the change
		// Just like with matrix math
		this->m_qOrientation *= qChange;

// Which is exactly the same as:
//		this->m_qOrientation = this->m_qOrientation * qChange;
	}

	glm::quat get_qOrientation(void)
	{
		return this->m_qOrientation;
	}

	glm::vec3 drawScale;
	void setUniformDrawScale(float scale);
	bool bIsVisible;

	bool bUseDebugColours;
	glm::vec4 wholeObjectDebugColourRGBA;

	bool bIsWireframe;
	bool bDoNotLight;

	std::vector<cMesh*> vec_pChildMeshes;

	//void Update(double deltaTime);

	unsigned int getUniqueID(void);

private:
	unsigned int m_UniqueID;
	static const unsigned int FIRST_UNIQUE_ID = 1000;
	static unsigned int m_nextUniqueID;
	glm::quat m_qOrientation;
};

#endif

