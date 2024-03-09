#pragma once
#include <string>
#include "../PandaEngine/GraphicsCommon.h"
#include <vector>
class Texture
{
public:
	Texture();
	~Texture();
	std::string getTextureName(void);
	bool LoadTexture(std::string textureName, std::string fileNameFullPath, bool bGenerateMIPMap);
	bool LoadCubeMap(std::string cubeMapName,
				std::string posX_fileName, std::string negX_fileName,
				std::string posY_fileName, std::string negY_fileName,
				std::string posZ_fileName, std::string negZ_fileName,
				bool bIsSeamless, std::string &errorString);
	GLuint getTextureNumber(void) { return this->textureNumber; }
private:
	std::string textureName;
	std::string fileNameFullPath;
	bool bGenerateMIPMap;
	GLuint textureNumber;
	bool m_bIsCubeMap;
	int width, height, nrChannels;
	unsigned char* data;

	void ClearTexture(void);
};

