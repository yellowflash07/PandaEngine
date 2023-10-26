//https://www.glfw.org/docs/latest/quick.html
//http://graphics.stanford.edu/data/3Dscanrep/

#include "../PandaEngine/GraphicsCommon.h"


#include <iostream>
#include <fstream>      // C++ file IO (secret: it's a wraper for the c IO)
#include <sstream>      // like a string builder
#include <vector>       // Aka a "smart array"


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


cLightManager* g_pTheLights = NULL;
int g_selectedMesh = 0;

// https://stackoverflow.com/questions/5289613/generate-random-float-between-two-floats
float getRandomFloat(float a, float b) {
    float random = ((float)rand()) / (float)RAND_MAX;
    float diff = b - a;
    float r = random * diff;
    return a + r;
}


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

    window = glfwCreateWindow(640, 480, "Simple example", NULL, NULL);
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

    ::g_pTheLights = new cLightManager();
    ::g_pTheLights->SetUniformLocations(shaderProgramID);

    ::g_pTheLights->theLights[0].param2.x = 1.0f;   // Turn on
//    ::g_pTheLights->theLights[0].param1.x = 0.0f;   // 0 = point light
    ::g_pTheLights->theLights[0].param1.x = 1.0f;   // 1 = spot light
    // With spots, set direction (relative)
    ::g_pTheLights->theLights[0].direction = glm::vec4(0.0f, -1.0f, 0.0f, 1.0f);
    //	vec4 param1;	// x = lightType, y = inner angle, z = outer angle, w = TBD
    ::g_pTheLights->theLights[0].param1.y = 15.0f;
    ::g_pTheLights->theLights[0].param1.z = 25.0f;


    ::g_pTheLights->theLights[0].position.x = -46.0f;
    ::g_pTheLights->theLights[0].position.y = 23.0f;
    ::g_pTheLights->theLights[0].position.z = -26.0f;



    // How "bright" the lights is
    // Really the opposite of brightness.
    //  how dim do you want this
    ::g_pTheLights->theLights[0].atten.x = 0.0f;        // Constant attenuation
    ::g_pTheLights->theLights[0].atten.y = 0.01f;        // Linear attenuation
    ::g_pTheLights->theLights[0].atten.z = 0.01f;        // Quadratic attenuation

    // Light #1 is a directional light 
    ::g_pTheLights->theLights[1].param2.x = 1.0f;   // Turn on
    ::g_pTheLights->theLights[1].param1.x = 2.0f;   // Directional

    // Direction with respect of the light.
    ::g_pTheLights->theLights[1].direction = glm::normalize(glm::vec4(-0.5f, -1.0f, 0.0f, 1.0f));
    //    float lightBrightness = 0.7f;
    ::g_pTheLights->theLights[1].diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    ::g_pTheLights->theLights[1].specular = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);


    //    glm::vec3 cameraEye = glm::vec3(10.0, 5.0, -15.0f);
    float yaxisRotation = 0.0f;

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


        // *****************************************************************

        ::g_pTheLights->UpdateUniformValues(shaderProgramID);


        // *****************************************************************
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
        // We should add this to a list of times, and get the average frame time
        const double LARGEST_DELTA_TIME = 1.0f / 30.0f; // 30FPS  (16 ms)
        if (deltaTime > LARGEST_DELTA_TIME)
        {
            deltaTime = LARGEST_DELTA_TIME;
        }
        //        std::cout << deltaTime << std::endl;
        lastTime = currentTime;


        // Point the spotlight at the bathtub
        cMesh* pBathTub = meshManager->FindMeshByFriendlyName("bathtub");
        if (pBathTub)
        {
            glm::vec3 bathTubToLightRay = pBathTub->drawPosition - glm::vec3(::g_pTheLights->theLights[0].position);

            bathTubToLightRay = glm::normalize(bathTubToLightRay);

            ::g_pTheLights->theLights[0].direction = glm::vec4(bathTubToLightRay, 1.0f);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();


        std::string theTitle = "Friendly robos";

        glfwSetWindowTitle(window, theTitle.c_str());

    }


    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}

