#pragma once

#include "../cVAOManager/cVAOManager.h"
#include "../PandaEngine/GraphicsCommon.h"
#include "cMesh.h"
#include <string>
#include <vector>
#include "SceneSaver.h"

class MeshManager
{
public:
	MeshManager();
	~MeshManager();

	cMesh* AddMesh(std::string modelNameAtPath, std::string friendlyName, unsigned int shaderProgramID);
	void DrawObject(cMesh* pCurrentMesh, glm::mat4 matModelParent, GLuint shaderProgramID);
	void DrawAllObjects(GLuint shaderProgramID);
	void SetBasePath(std::string basePath);
	cMesh* FindMeshByFriendlyName(std::string friendlyNameToFind);
	void DrawTransformBox();
	void LoadSavedMeshes(unsigned int shaderProgramID);
private:
	cVAOManager* vaoManager;
	std::vector< cMesh* > meshList;
	cMesh* selectedMesh;
	SceneSaver saver;
};