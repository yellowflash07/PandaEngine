#pragma once

#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>
#include "../Basic Shader Manager/cShaderManager.h"
#include "../PandaEngine/GraphicsCommon.h"
#include "../cVAOManager/sModelDrawInfo.h"

struct DebugLine
{
	glm::vec3 start;
	glm::vec3 end;
	glm::vec3 color0;
	glm::vec3 color1;

	GLuint vao;
	GLuint vbo;

	sVertex* vertices;

	DebugLine(glm::vec3 start, glm::vec3 end, glm::vec3 color0, glm::vec3 color1)
		: start(start), end(end), color0(color0), color1(color1)
	{
		vertices = new sVertex[2];
		vertices[0].x = start.x;
		vertices[0].y = start.y;
		vertices[0].z = start.z;
		vertices[0].r = color0.x;
		vertices[0].g = color0.y;
		vertices[0].b = color0.z;

		vertices[1].x = end.x;
		vertices[1].y = end.y;
		vertices[1].z = end.z;
		vertices[1].r = color1.x;
		vertices[1].g = color1.y;
		vertices[1].b = color1.z;
	}
};

class DebugRenderer
{
public:
	DebugRenderer();
	~DebugRenderer();

	void DrawLine(glm::vec3 start, glm::vec3 end, glm::vec3 color0, glm::vec3 color1);

	static DebugRenderer* getInstance();

private:
	GLuint shaderProgramID;
	std::vector<DebugLine> lines;
	void LoadLineToGPU(DebugLine& line);
};

