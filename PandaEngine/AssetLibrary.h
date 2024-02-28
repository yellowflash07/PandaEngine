#pragma once

#include <vector>
#include <string>
#include "../TextureManager/cBasicTextureManager.h"
#include "MeshManager.h"
#include "RenderTexture.h"

class AssetLibrary
{
public:
	AssetLibrary();
	~AssetLibrary();

	void Init();
	void RenderBox();
	std::vector<std::wstring> GetFiles(std::string folder);

	cBasicTextureManager* m_texManager;
	MeshManager* m_meshManager;
	GLuint shaderProgramID;
private:
	std::vector<std::wstring> m_modelFiles;
	std::vector<std::wstring> m_texFiles;
	std::vector<GLuint> m_texIDs;
	std::vector<cMesh*> m_meshes;
	std::vector<RenderTexture*> m_renderTextures;
};

