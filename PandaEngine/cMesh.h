#ifndef _cMesh_HG_
#define _cMesh_HG_

#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>
#include <string>
#include <vector>
#include "../cVAOManager/sModelDrawInfo.h"
#include "../PandaEngine/GraphicsCommon.h"
#include "TransformComponent.h"
#include "IEditorUI.h"

class cMesh : public IEditorUI
{
public:
	cMesh();

	cMesh(std::string meshName, std::string friendlyName);
	~cMesh();		

	std::string meshName;
	std::string uniqueName;
	std::string friendlyName;		

	TransformComponent transform;


	bool bIsVisible;

	bool bUseDebugColours;
	glm::vec4 wholeObjectDebugColourRGBA;
	glm::vec4 color;
	bool bIsWireframe;
	bool bDoNotLight;
	bool isSkyBox;
	bool hasVertexColors;
	bool isReflective;
	static const int NUM_OF_TEXTURES = 4;
	std::string texture[NUM_OF_TEXTURES];
	float textureRatio[NUM_OF_TEXTURES];
	std::string maskTexture;
	GLuint renderTextureID =0;
	glm::vec2 UV_Offset;
	float transperancy = 1.0f;
	std::vector<cMesh*> vec_pChildMeshes;
	unsigned int getUniqueID(void);

	glm::vec3 maxExtents_XYZ;	// bounding box maximums
	glm::vec3 minExtents_XYZ;	// bounding box minimums

	void calcExtents(void);

	glm::vec3 GetCenter(void)
	{
		return (this->maxExtents_XYZ + this->minExtents_XYZ) / 2.0f;
	}

	
	std::vector<sModelDrawInfo> modelDrawInfo;

	bool isChild = false;
	void AddChild(cMesh* child);
	bool hideParent = false;
	bool useBone = false;
	//glm::mat4 AnimatedTransform;

	void Render();
private:
	unsigned int m_UniqueID;
	static const unsigned int FIRST_UNIQUE_ID = 1000;
	static unsigned int m_nextUniqueID;
	void LoadMesh(std::string fileName, std::string friendlyName);
};

#endif

