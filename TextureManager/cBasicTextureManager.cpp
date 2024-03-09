#include "cBasicTextureManager.h"

#include <sstream>

void cBasicTextureManager::SetBasePath(std::string basepath)
{
	this->m_basePath = basepath;
	return;
}


bool cBasicTextureManager::Create2DTextureFromBMPFile( std::string textureFileName, bool bGenerateMIPMap )
{
	std::string fileToLoadFullPath = this->m_basePath + "/" + textureFileName;

	Texture* pTempTexture = new Texture();

	if (!pTempTexture->LoadTexture(textureFileName, fileToLoadFullPath, bGenerateMIPMap))
	{
		this->m_appendErrorString( "Can't load " );
		this->m_appendErrorString( fileToLoadFullPath );
		this->m_appendErrorString( "\n" );
		return false;
	}

	//CTextureFromBMP* pTempTexture = new CTextureFromBMP();
	//if ( ! pTempTexture->CreateNewTextureFromBMPFile2( textureFileName, fileToLoadFullPath, /*textureUnit,*/ bGenerateMIPMap ) )
	//{
	//	this->m_appendErrorString( "Can't load " );
	//	this->m_appendErrorString( fileToLoadFullPath );
	//	this->m_appendErrorString( "\n" );
	//	return false;
	//}

	// Texture is loaded OK
	//this->m_nextTextureUnitOffset++;
	
	this->textureMap[ textureFileName ] = pTempTexture;

	return true;
}

void cBasicTextureManager::m_appendErrorString( std::string nextErrorText )
{
	std::stringstream ss;
	ss << this->m_lastError << nextErrorText;
	this->m_lastError = ss.str();
	return;
}

GLuint cBasicTextureManager::getTextureIDFromName( std::string textureFileName )
{
	std::map< std::string, Texture* >::iterator itTexture
		= this->textureMap.find( textureFileName );
	// Found it?
	if ( itTexture == this->textureMap.end() )
	{
		return 0;
	}
	// Reutrn texture number (from OpenGL genTexture)
	return itTexture->second->getTextureNumber();
}


void cBasicTextureManager::m_appendErrorStringLine( std::string nextErrorTextLine )
{
	std::stringstream ss;
	ss << this->m_lastError << std::endl;
	ss << nextErrorTextLine << std::endl;
	this->m_lastError = ss.str();
	return;
}


// Picks a random texture from the textures loaded
std::string cBasicTextureManager::PickRandomTexture(void)
{
	return "";
	//if ( this->textureMap.empty() )
	//{
	//	// There are no textures loaded, yet.
	//	return "";
	//}

	//int textureIndexRand = rand() % (this->textureMap.size() + 1);
	//if ( textureIndexRand >= this->textureMap.size() )
	//{
	//	textureIndexRand = 0;
	//}

	//std::map< std::string, Texture* >::iterator itTex = this->textureMap.begin();
	//for ( unsigned int count = 0; count != textureIndexRand; count++, itTex++ ) 
	//{}

	//return itTex->second->getTextureName();
}


bool cBasicTextureManager::CreateCubeTextureFromBMPFiles( 
                                    std::string cubeMapName, 
		                            std::string posX_fileName, std::string negX_fileName, 
		                            std::string posY_fileName, std::string negY_fileName, 
									std::string posZ_fileName, std::string negZ_fileName, 
									bool bIsSeamless, std::string &errorString )
{
	std::string posX_fileName_FullPath = this->m_basePath + "/" + posX_fileName;
	std::string negX_fileName_FullPath = this->m_basePath + "/" + negX_fileName;
	std::string posY_fileName_FullPath = this->m_basePath + "/" + posY_fileName;
	std::string negY_fileName_FullPath = this->m_basePath + "/" + negY_fileName;
	std::string posZ_fileName_FullPath = this->m_basePath + "/" + posZ_fileName;
	std::string negZ_fileName_FullPath = this->m_basePath + "/" + negZ_fileName;

	GLenum errorEnum;
	std::string errorDetails;
	Texture* pTempTexture = new Texture();
	if ( ! pTempTexture->LoadCubeMap( 
				cubeMapName, 
				posX_fileName_FullPath, negX_fileName_FullPath, 
	            posY_fileName_FullPath, negY_fileName_FullPath, 
	            posZ_fileName_FullPath, negZ_fileName_FullPath, 
	            bIsSeamless, errorString) )
	{
		this->m_appendErrorString( "Can't load " );
		this->m_appendErrorString( cubeMapName );
		this->m_appendErrorString( " because:\n" );
		this->m_appendErrorString( errorString );
		this->m_appendErrorString( "\n" );
		this->m_appendErrorString( errorDetails );
		errorString += ("\n" + errorDetails);
		return false;
	}//if ( ! pTempTexture->CreateNewCubeTextureFromBMPFiles()

	// Texture is loaded OK
	//this->m_nextTextureUnitOffset++;
	
	this->textureMap[ cubeMapName ] = pTempTexture;

	return true;
}

