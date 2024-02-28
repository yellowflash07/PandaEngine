#include "RenderTexture.h"

RenderTexture::RenderTexture(Camera* camera, int width, int height, GLuint shaderProgramID, std::vector<cMesh*> meshList)
{
	std::string error;
	this->camera = camera;
	this->fbo = new cFBO();
	if (!this->fbo->init(width, height, error))
	{
		std::cout << "Error initializing FBO: " << error << std::endl;
	}
	this->shaderProgramID = shaderProgramID;
	isRendering = false;
	this->offScreenMeshList = meshList;
}

RenderTexture::~RenderTexture()
{
	delete this->fbo;
}

void RenderTexture::Render()
{
	isRendering = true;
	glBindFramebuffer(GL_FRAMEBUFFER, fbo->ID);

	fbo->clearBuffers(true, true);

	float fboRatio = fbo->width / (float)fbo->height;
	glViewport(0, 0, fbo->width, fbo->height);

	glm::vec3 sceneEye = this->camera->cameraEye;
	glm::vec3 sceneTarget = this->camera->cameraTarget + sceneEye;
	glm::vec3 upVector = this->camera->upVector;

	glm::mat4 matProjection = glm::perspective(0.6f,
												fboRatio,
												0.1f,
												10'000.0f);

	glm::mat4 matView = glm::lookAt(sceneEye,
									sceneTarget,
									upVector);

	GLint matProjection_UL = glGetUniformLocation(shaderProgramID, "matProjection");
	glUniformMatrix4fv(matProjection_UL, 1, GL_FALSE, glm::value_ptr(matProjection));

	GLint matView_UL = glGetUniformLocation(shaderProgramID, "matView");
	glUniformMatrix4fv(matView_UL, 1, GL_FALSE, glm::value_ptr(matView));

	//lightManager->UpdateLights(shaderProgramID);

	//draw meshes
	for (int i = 0; i < offScreenMeshList.size(); i++)
	{
		meshManager->DrawObject(offScreenMeshList[i], glm::mat4(1.0), shaderProgramID);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GLuint RenderTexture::GetTextureID()
{
	return isRendering ? this->fbo->colourTexture_0_ID : 0;
}

GLuint RenderTexture::GetFBOID()
{
	return this->fbo->ID;
}
