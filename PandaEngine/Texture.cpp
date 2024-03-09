#include "Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include "../TextureManager/stb_image.h"
#include <iostream>

Texture::Texture()
{
}

bool Texture::LoadTexture(std::string textureName, std::string fileNameFullPath, bool bGenerateMIPMap)
{
	
	data = stbi_load(fileNameFullPath.c_str(), &width, &height, &nrChannels, 0);
	if (data)
	{
		glGenTextures(1, &textureNumber);
		glBindTexture(GL_TEXTURE_2D, textureNumber);
		if (nrChannels == 4)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		}
		else if (nrChannels == 3)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		}
		else
		{
			std::cout << "Error: Texture " << fileNameFullPath << " has " << nrChannels << " channels." << std::endl;
			return false;
		}

		if (bGenerateMIPMap)
		{
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		}
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		//stbi_image_free(data);
		return true;
	}
	else
	{
		std::cout << "Error: Texture " << fileNameFullPath << " failed to load." << std::endl;
		return false;
	}
}

bool Texture::LoadCubeMap(std::string cubeMapName, std::string posX_fileName, std::string negX_fileName, std::string posY_fileName, std::string negY_fileName, std::string posZ_fileName, std::string negZ_fileName, bool bIsSeamless, std::string& errorString)
{
	bool bReturnVal = true;

	// Do the magic...

	// Pick a texture number...
	//GLuint textureNum = 0;
	glGenTextures(1, &(this->textureNumber));
	// Worked?
	if ((glGetError() & GL_INVALID_VALUE) == GL_INVALID_VALUE)
	{
		bReturnVal = false;
		return false;
	}

	//
	//glBindTexture(GL_TEXTURE_2D, this->m_textureNumber );
	glBindTexture(GL_TEXTURE_CUBE_MAP, this->textureNumber);

	//glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_GENERATE_MIPMAP, GL_TRUE);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE /*GL_REPEAT*/);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE /*GL_REPEAT*/);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE /*GL_REPEAT*/);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// IF YOU DON'T GENERATE MIP MAPS, then you can use LINEAR filtering
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );

	if (bIsSeamless)
	{
		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);	// OpenGL 4.1, maybe
	}

	//if (this->bWasThereAnOpenGLError(errorEnum, errorString, errorDetails)) { return false; }

	// Positive X image...
	// Assume all the images are the same size. If not, then it will screw up
	if (this->LoadTexture("PosX", posX_fileName, false))
	{
		glTexStorage2D(GL_TEXTURE_CUBE_MAP,
			10, // Mipmap levels
			GL_RGBA8,	// Internal format
			this->width,	// width (pixels)
			this->height);		// height (pixels)

		//if (this->bWasThereAnOpenGLError(errorEnum, errorString, errorDetails)) { return false; }
	}
	else
	{
		//this->m_lastErrorNum = CTextureFromBMP::ERORR_FILE_WONT_OPEN;
		return false;
	}

	// Positive X image...
	glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X,
		0,   // Level
		0, 0, // Offset
		this->width,	// width
		this->height,		// height
		GL_RGB,
		GL_UNSIGNED_BYTE,
		this->data);
	this->ClearTexture();
	//this->ClearBMP();
	//if (this->bWasThereAnOpenGLError(errorEnum, errorString, errorDetails)) { return false; }


	// Negative X image...
	if (this->LoadTexture("NegX", negX_fileName, false))
	{
		glTexSubImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, 0, 0,
			this->width, this->height, GL_RGB, GL_UNSIGNED_BYTE, this->data);
		this->ClearTexture();
		//this->ClearBMP();
		//if (this->bWasThereAnOpenGLError(errorEnum, errorString, errorDetails)) { return false; }
	}
	else
	{
		//this->m_lastErrorNum = CTextureFromBMP::ERORR_FILE_WONT_OPEN;
		return false;
	}

	// Positive Y image...
	if (this->LoadTexture("PosY", posY_fileName, false))
	{
		glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, 0, 0, 
			this->width, this->height, GL_RGB, GL_UNSIGNED_BYTE, this->data);
		this->ClearTexture();
		//this->ClearBMP();
		//if (this->bWasThereAnOpenGLError(errorEnum, errorString, errorDetails)) { return false; }
	}
	else
	{
		//this->m_lastErrorNum = CTextureFromBMP::ERORR_FILE_WONT_OPEN;
		return false;
	}

	// Negative Y image...
	if (this->LoadTexture("NegY", negY_fileName, false))
	{
		glTexSubImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, 0, 0, 
			this->width, this->height, GL_RGB, GL_UNSIGNED_BYTE, this->data);
		this->ClearTexture();//this->ClearBMP();
		//if (this->bWasThereAnOpenGLError(errorEnum, errorString, errorDetails)) { return false; }
	}
	else
	{
		//this->m_lastErrorNum = CTextureFromBMP::ERORR_FILE_WONT_OPEN;
		return false;
	}

	// Positive Z image...
	if (this->LoadTexture("PosZ", posZ_fileName, false))
	{

		glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, 0, 0, 
			this->width, this->height, GL_RGB, GL_UNSIGNED_BYTE, this->data);
		this->ClearTexture();//this->ClearBMP();
		//if (this->bWasThereAnOpenGLError(errorEnum, errorString, errorDetails)) { return false; }
	}
	else
	{
		//this->m_lastErrorNum = CTextureFromBMP::ERORR_FILE_WONT_OPEN;
		return false;
	}

	// Negative Z image...
	if (this->LoadTexture("NegZ", negZ_fileName, false))
	{
		glTexSubImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, 0, 0, 
			this->width, this->height, GL_RGB, GL_UNSIGNED_BYTE, this->data);
		this->ClearTexture();//this->ClearBMP();
		//if (this->bWasThereAnOpenGLError(errorEnum, errorString, errorDetails)) { return false; }
	}
	else
	{
		//this->m_lastErrorNum = CTextureFromBMP::ERORR_FILE_WONT_OPEN;
		//return false;
	}

	this->textureName = cubeMapName;

	//this->m_textureUnit = textureUnit;

	this->m_bIsCubeMap = true;

	// End of Do the magic...
	return bReturnVal;

}

void Texture::ClearTexture(void)
{
	if (this->data != 0)
	{
		stbi_image_free(this->data);
	}
	
}
