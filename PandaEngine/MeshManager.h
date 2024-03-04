#pragma once

#include "../cVAOManager/cVAOManager.h"
#include "../PandaEngine/GraphicsCommon.h"
#include "../TextureManager/cBasicTextureManager.h"
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
	cMesh* LoadMesh(std::string modelNameAtPath, std::string friendlyName, unsigned int shaderProgramID);
	void DrawObject(cMesh* pCurrentMesh, glm::mat4 matModelParent, GLuint shaderProgramID);
	void DrawAllObjects(GLuint shaderProgramID);
	void SetBasePath(std::string basePath);
	void SetTexturePath(std::string texturePath);
	cMesh* FindMeshByFriendlyName(std::string friendlyNameToFind);
	void DrawTransformBox();
	void LoadSavedMeshes(unsigned int shaderProgramID);
	bool GetModelDrawInfo(std::string friendlyName, sModelDrawInfo& drawInfo);
	bool GetTransformedMeshDrawInfo(std::string friendlyName, sModelDrawInfo& drawInfo);
	void ToggleWireframe(bool wireframe);
	bool LoadTexture(std::string textureFileName);
	bool LoadCubeMap(std::string cubeMapName, std::string posX_fileName, std::string negX_fileName,
				std::string posY_fileName, std::string negY_fileName,
				std::string posZ_fileName, std::string negZ_fileName, bool bIsSeamless);
	cBasicTextureManager* GetTextureManager() { return textureManager; }
	void RemoveMesh(std::string friendlyName);
	cVAOManager* vaoManager;
	void UpdateVAOBuffers(std::string friendlyName, sModelDrawInfo& drawInfo);
private:
	GLuint shaderProgramID;
	std::vector< cMesh* > meshList;
	cMesh* selectedMesh;
	SceneSaver* saver;
	cBasicTextureManager* textureManager;
	std::string basePath;
	void SetUpTextures(cMesh* pCurrentMesh, GLuint shaderProgramID);
	void CalculateMatrices(Node* node, const glm::mat4& parentTransformationMatrix, sModelDrawInfo &modelInfo);
};