#pragma once
#include "../PandaEngine/GraphicsCommon.h"
#include <glm/vec3.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp> 
class Camera
{
public:
	Camera(glm::vec3 position,
		glm::vec3 target, 
		glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f),
		float near = 0.1f, 
		float far = 1000.0f);
	~Camera();

	void Update(GLFWwindow* window, double deltaTime);
	void ProcessMouseMovement(double xpos, double ypos);
	void ProcessKeyboardInput(GLFWwindow* window, double deltaTime);
	GLuint shaderProgramID;
	void SetPosition(glm::vec3 position) { cameraEye = position; }
	void Follow(glm::vec3 followPos,glm::vec3 offset, glm::vec3 followTarget, glm::vec3 followOrientation = glm::vec3(0.0));
	glm::vec3 GetCameraRotation();

	glm::vec3 cameraEye;
	glm::vec3 cameraTarget;
	glm::vec3 upVector;
	bool isFollowing = false;
	bool camControl = true;
	glm::vec3 GetForwardVector();
	glm::vec3 GetRightVector();
	glm::vec3 GetUpVector();

	glm::mat4 matProjection;
	glm::mat4 matView;

	glm::mat4 GetViewMatrix();
	glm::mat4 GetProjectionMatrix();
	glm::mat4 GetViewProjectionMatrix();
	float speed;
	float yaw;
	float pitch;
private:

	float near;
	float far;
	
	bool stopUpdates; 

	glm::vec3 followPos;
	glm::vec3 followTarget;
	glm::vec3 followOrientation;
	glm::vec3 offset;

	glm::vec3 forwardVector;
	glm::vec3 rightVector;
};

