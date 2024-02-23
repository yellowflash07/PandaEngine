#pragma once

#include "Camera.h"
#include "../FBO/cFBO.h"
#include <string>
#include <iostream>
#include "../PandaEngine/GraphicsCommon.h"
#include "MeshManager.h"
#include "cLightManager.h"

class RenderTexture
{
public:
	RenderTexture(Camera* camera, int width, int height, GLuint shaderProgramID, std::vector<cMesh*> meshList);
	~RenderTexture();

	void Render();
	GLuint GetTextureID();
	GLuint GetFBOID();
	MeshManager* meshManager;
	cLightManager* lightManager;
	std::vector<cMesh*> offScreenMeshList;

private:
	Camera* camera;
	cFBO* fbo;
	GLuint shaderProgramID;
	bool isRendering;
};

