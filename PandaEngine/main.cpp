
#include <iostream>
#include "GraphicsCommon.h"
#include "../Basic Shader Manager/cShaderManager.h"

#include <fstream>


int main()
{
	if (!glfwInit())
	{
		return 1;
	}

	GLFWwindow* window = glfwCreateWindow(640, 480, "Simple example", NULL, NULL);
	if (!window)
	{
		return 1;
	}

	glfwMakeContextCurrent(window);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	//glEnable(GL_DEPTH_TEST);

	//STUFF

	float points[] = {
					   0.0f,  0.5f,  0.0f,
					   0.5f, -0.5f,  0.0f,
					  -0.5f, -0.5f,  0.0f
	};

	GLuint vbo = 0;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(float), points, GL_STATIC_DRAW);

	GLuint vao = 0;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);


	cShaderManager shaderManager;
	shaderManager.setBasePath("../Assets/Shaders");

	cShaderManager::cShader vertexShader;
	vertexShader.fileName = "vertexshader.glsl";

	cShaderManager::cShader fragmentShader;
	fragmentShader.fileName = "fragmentshader.glsl";


	if (!shaderManager.createProgramFromFile("Simple shader", vertexShader, fragmentShader))
	{
		std::cout << "Error: Couldn't compile or link:" << std::endl;
		std::cout << shaderManager.getLastError();
		return -1;
	}
	


	while (!glfwWindowShouldClose(window)) {
		// wipe the drawing surface clear
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		shaderManager.useShaderProgram(shaderManager.getIDFromFriendlyName("Simple shader"));
	//	glUseProgram(shader_programme);
		glBindVertexArray(vao);
		// draw points 0-3 from the currently bound VAO with current in-use shader
		glDrawArrays(GL_TRIANGLES, 0, 3);
		// update other events like input handling 
		glfwPollEvents();
		// put the stuff we've been drawing onto the display
		glfwSwapBuffers(window);
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
	return 0;
}