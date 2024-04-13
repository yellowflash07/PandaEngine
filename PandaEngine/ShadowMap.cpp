#include "ShadowMap.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>

ShadowMap::ShadowMap()
{
	m_fbo = 0;
	m_shadowMap = 0;
	m_shadowWidth = 1024;
	m_shadowHeight = 1024;
}

ShadowMap::~ShadowMap()
{
}

bool ShadowMap::Initialize(int width, int height)
{
	m_shadowWidth = width;
	m_shadowHeight = height;
	glGenFramebuffers(1, &m_fbo);
	glGenTextures(1, &m_shadowMap);
	glBindTexture(GL_TEXTURE_2D, m_shadowMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_shadowWidth, m_shadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	// Prevents darkness outside the frustrum
	float clampColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, clampColor);

	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_shadowMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return true;
}

void ShadowMap::BeginRender(cLight* light)
{
	GLint isShadowMap = glGetUniformLocation(shaderProgramID, "isShadowMap");
	glUniform1i(isShadowMap, GL_TRUE);

	glViewport(0, 0, m_shadowWidth, m_shadowHeight);
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	glClear(GL_DEPTH_BUFFER_BIT); 
	glEnable(GL_CULL_FACE);
	// (Usually) the default - does NOT draw "back facing" triangles
	glCullFace(GL_FRONT);

	float near_plane = 1.0f, far_plane = 1000.0f;
	float orthoSize = 1000.0f;
	glm::mat4 lightProjection = glm::ortho(-orthoSize, orthoSize, -orthoSize, orthoSize, 0.1f, far_plane);

	glm::vec3 lightPos = glm::vec3(light->position);
	//light->position = glm::vec4(lightPos,1.0f);
	glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0), glm::vec3(0.0, 1.0, 0.0));

	lightSpaceMatrix = lightProjection * lightView;

	GLint lightSpaceMatrix_UL = glGetUniformLocation(shaderProgramID, "lightSpaceMatrix");
	glUniformMatrix4fv(lightSpaceMatrix_UL, 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
	

}

void ShadowMap::EndRender()
{

	GLint isShadowMap = glGetUniformLocation(shaderProgramID, "isShadowMap");
	glUniform1i(isShadowMap, GL_FALSE);

	glActiveTexture(GL_TEXTURE0 + 69);
	glBindTexture(GL_TEXTURE_2D, m_shadowMap);
	GLint shadowMap = glGetUniformLocation(shaderProgramID, "shadowMap");
	glUniform1i(shadowMap, 69);


	GLint lightSpaceMatrix_UL = glGetUniformLocation(shaderProgramID, "lightSpaceMatrix");
	glUniformMatrix4fv(lightSpaceMatrix_UL, 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));

	glCullFace(GL_BACK);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, 1920, 1080);
}
