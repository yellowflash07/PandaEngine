
#include "../PandaEngine/GraphicsCommon.h"

#include <iostream>
#include <fstream>     
#include <sstream>      
#include <vector>      

//#include "linmath.h"
#include <glm/glm.hpp>
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> 
// glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr

#include <stdlib.h>
#include <stdio.h>
#include <string>

#include "../Basic Shader Manager/cShaderManager.h"
#include "MeshManager.h"
#include "cLightManager.h"
#include "cLightHelper.h"

glm::vec3 g_cameraEye = glm::vec3(0.0, 70.0, 181.0f);
glm::vec3 g_cameraTarget = glm::vec3(0.0f, 5.0f, 0.0f);
glm::vec3 g_upVector = glm::vec3(0.0f, 1.0f, 0.0f);


static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}


int main(void)
{

    GLFWwindow* window;
    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    window = glfwCreateWindow(640, 480, "Template Scene", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }


    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glfwSwapInterval(1);


    cShaderManager* pShaderThing = new cShaderManager();
    pShaderThing->setBasePath("../Assets/Shaders");

    cShaderManager::cShader vertexShader;
    vertexShader.fileName = "vertexShader.glsl";

    cShaderManager::cShader fragmentShader;
    fragmentShader.fileName = "fragmentShader.glsl";

    if (!pShaderThing->createProgramFromFile("shader01", vertexShader, fragmentShader))
    {
        std::cout << "Error: Couldn't compile or link:" << std::endl;
        std::cout << pShaderThing->getLastError();
        return -1;
    }

    GLuint shaderProgramID = pShaderThing->getIDFromFriendlyName("shader01");

    MeshManager* meshManager = new MeshManager();
    meshManager->SetBasePath("../Assets/Models");
    cMesh* bathtub = meshManager->AddMesh("bathtub_xyz_n_rgba.ply", "bathtub", shaderProgramID);
    bathtub->drawPosition = glm::vec3(0.0f, -30.0f, 0.0f);

    cLightManager* lightManager = new cLightManager();
    lightManager->SetUniformLocations(shaderProgramID);

    //Light #1 is a directional light 
    lightManager->theLights[1].param2.x = 1.0f;   // Turn on
    lightManager->theLights[1].param1.x = 2.0f;   // Directional

    //Direction with respect of the light.
    lightManager->theLights[1].direction = glm::normalize(glm::vec4(-0.5f, -1.0f, 0.0f, 1.0f));
    lightManager->theLights[1].diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    lightManager->theLights[1].specular = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

    double lastTime = glfwGetTime();

    while (!glfwWindowShouldClose(window))
    {

        float ratio;
        int width, height;

        glUseProgram(shaderProgramID);

        glfwGetFramebufferSize(window, &width, &height);
        ratio = width / (float)height;

        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // While drawing a pixel, see if the pixel that's already there is closer or not?
        glEnable(GL_DEPTH_TEST);
        // (Usually) the default - does NOT draw "back facing" triangles
        glCullFace(GL_BACK);

        lightManager->UpdateUniformValues(shaderProgramID);

        GLint eyeLocation_UL = glGetUniformLocation(shaderProgramID, "eyeLocation");
        glUniform4f(eyeLocation_UL,
            ::g_cameraEye.x, ::g_cameraEye.y, ::g_cameraEye.z, 1.0f);

        glm::mat4 matProjection = glm::perspective(0.6f,
            ratio,
            0.1f,
            1000.0f);

        glm::mat4 matView = glm::lookAt(::g_cameraEye,
            ::g_cameraTarget,
            ::g_upVector);

        GLint matProjection_UL = glGetUniformLocation(shaderProgramID, "matProjection");
        glUniformMatrix4fv(matProjection_UL, 1, GL_FALSE, glm::value_ptr(matProjection));

        GLint matView_UL = glGetUniformLocation(shaderProgramID, "matView");
        glUniformMatrix4fv(matView_UL, 1, GL_FALSE, glm::value_ptr(matView));


        // *********************************************************************       
        meshManager->DrawAllObjects(shaderProgramID);

        // Time per frame (more or less)
        double currentTime = glfwGetTime();
        double deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}

