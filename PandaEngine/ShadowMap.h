#pragma once

#include "../PandaEngine/GraphicsCommon.h";
#include "cLightManager.h"
class ShadowMap
{
public:
	ShadowMap();
	~ShadowMap();

	bool Initialize(int width, int height);

	void BeginRender(cLight* light);
	void EndRender();
	GLint shaderProgramID;

	GLuint m_shadowMap;
	GLFWwindow* window;
private:
	GLuint m_fbo;
	int m_shadowWidth;
	int m_shadowHeight;
	glm::mat4 lightSpaceMatrix;
};

