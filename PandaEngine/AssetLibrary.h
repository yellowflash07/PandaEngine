#pragma once

#include <vector>
#include <string>
#include <imgui.h>
#include "../TextureManager/cBasicTextureManager.h"
class AssetLibrary
{
public:
	AssetLibrary();
	~AssetLibrary();

	void Init();
	void LoadAssets();

	void RenderBox();
	std::vector<std::wstring> GetFiles(std::string folder);

	cBasicTextureManager* m_texManager;

private:
	std::vector<std::wstring> m_modelFiles;
	std::vector<std::wstring> m_texFiles;
	std::vector<GLuint> m_texIDs;

};

