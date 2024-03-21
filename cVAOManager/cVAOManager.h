#ifndef _cVAOManager_HG_
#define _cVAOManager_HG_

// Will load the models and place them 
// into the vertex and index buffers to be drawn

#include <string>
#include <map>

#include "sModelDrawInfo.h"
#include <assimp/scene.h> 
#include "../PandaEngine/GraphicsCommon.h"

#include <Windows.h>

#include <functional>
#define WINDOWS_LEAN_AND_MEAN

typedef std::function<void(sModelDrawInfo)> OnModelLoadCallBack;

struct LoadInfo;



class cVAOManager
{
public:
	cVAOManager();

	static cVAOManager* getInstance();

	bool LoadModelIntoVAOAI(std::string fileName,
		sModelDrawInfo& drawInfo,
		unsigned int shaderProgramID,
		bool bIsDynamicBuffer = false);

	bool LoadModelIntoVAOAsync(std::string fileName,
		sModelDrawInfo& drawInfo,
		unsigned int shaderProgramID,
		OnModelLoadCallBack callback,
		bool bIsDynamicBuffer = false);

	// We don't want to return an int, likely
	bool FindDrawInfoByModelName(std::string filename,
								 sModelDrawInfo &drawInfo);

	std::string getLastError(bool bAndClear = true);

	void setBasePath(std::string basePathWithoutSlash);

	// Here you have to be a little careful about what you pass. 
	// It will use the vertex and element (triangle) buffer IDs 
	//	to update - i.e. OVERWRITE - the buffers with whatever
	//	you've got in the two pointers. 
	// You can get this information by calling FindDrawInfoByModelName()
	//	and using the drawInfo that is returned. Note the sDrawInfo is a
	//	COPY of the structure in the map, but the pointers to the 
	//	vertex and element buffers are the ONE AND ONLY COPIES that
	//	exist on the CPU side. So if you overwrite them, they're gone.
	bool UpdateVAOBuffers(std::string fileName,
						  sModelDrawInfo& updatedDrawInfo,
						  unsigned int shaderProgramID);

	void PrintMatrix(glm::mat4 theMatrix);
	void PrintMatrix(aiMatrix4x4 theMatrix);

	bool m_LoadTheFile(std::string fileName, sModelDrawInfo& drawInfo);

	void LoadVertexToGPU(sModelDrawInfo& drawInfo, GLuint shaderProgram);

	std::string GenerateUniqueModelNameFromFile(std::string fileName);

	std::map< std::string /*model name*/,
		sModelDrawInfo /* info needed to draw*/ >
		m_map_ModelName_to_VAOID;

	void CheckQueue();
private:


	std::string m_basePathWithoutSlash;

	void AssimpToGLM(const aiMatrix4x4 &fromAssimp, glm::mat4 &toGLM);
	Node* GenerateBoneHierarchy(const aiNode* node, sModelDrawInfo &drawInfo);

	//std::map<LoadInfo*, std::function<void()>> threadQueue;

	std::vector<LoadInfo*> loadQueue;

};

#endif	// _cVAOManager_HG_
