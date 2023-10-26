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
#include "../cVAOManager/cVAOManager.h"

#include "cMesh.h"
#include "cLightManager.h"
#include "cLightHelper.h"

glm::vec3 g_cameraEye = glm::vec3(0.0, 70.0, 181.0f);
glm::vec3 g_cameraTarget = glm::vec3(0.0f, 5.0f, 0.0f);
glm::vec3 g_upVector = glm::vec3(0.0f, 1.0f, 0.0f);

cVAOManager* g_pMeshManager = NULL;
cMesh* g_pDebugSphereMesh = NULL;
GLuint g_DebugSphereMesh_shaderProgramID = 0;

std::vector<double> g_vecLastFrameTimes;                                                    
std::vector< cMesh* > g_vec_pMeshesToDraw;
cMesh* g_pFindMeshByFriendlyName(std::string friendlyNameToFind);
cLightManager* g_pTheLights = NULL;

int g_selectedMesh = 0;
int g_selectedLight = 0;

void DrawObject(cMesh* pCurrentMesh, glm::mat4 matModel, GLuint shaderProgramID);

cMesh* g_pFindMeshByFriendlyName(std::string friendlyNameToFind);

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

    cMesh bob;

    std::cout << "About to blow you mind with OpenGL!" << std::endl;

    GLFWwindow* window;
    //    GLuint vertex_buffer; //, vertex_shader, fragment_shader;//v , program;
    //    GLint mvp_location;// , vpos_location, vcol_location;

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

   // glfwSetKeyCallback(window, key_callback);

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

    // Set the debug shader ID we're going to use
    ::g_DebugSphereMesh_shaderProgramID = shaderProgramID;

    ::g_pMeshManager = new cVAOManager();

    ::g_pMeshManager->setBasePath("../Assets/Models");


    sModelDrawInfo bathtubDrawingInfo;
    ::g_pMeshManager->LoadModelIntoVAO("bathtub_xyz_n_rgba.ply",
        bathtubDrawingInfo, shaderProgramID);
    std::cout << "Loaded: " << bathtubDrawingInfo.numberOfVertices << " vertices" << std::endl;

    cMesh* pBathtub = new cMesh();
    pBathtub->meshName = "bathtub_xyz_n_rgba.ply";
    pBathtub->friendlyName = "bathtub";
    pBathtub->drawPosition = glm::vec3(0.0f, -30.0f, 0.0f);
    ::g_vec_pMeshesToDraw.push_back(pBathtub);


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

        // Switch the "main" shader
//        shaderProgramID = pShaderThing->getIDFromFriendlyName("shader01");
//        glUseProgram(shaderProgramID);

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
                //uniform vec4 eyeLocation;
        GLint eyeLocation_UL = glGetUniformLocation(shaderProgramID, "eyeLocation");
        glUniform4f(eyeLocation_UL,
            ::g_cameraEye.x, ::g_cameraEye.y, ::g_cameraEye.z, 1.0f);



        //       //mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
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
        // Draw all the objects
        for (unsigned int index = 0; index != ::g_vec_pMeshesToDraw.size(); index++)
        {
            cMesh* pCurrentMesh = ::g_vec_pMeshesToDraw[index];

            if (pCurrentMesh->bIsVisible)
            {

                glm::mat4 matModel = glm::mat4(1.0f);   // Identity matrix

                DrawObject(pCurrentMesh, matModel, shaderProgramID);
            }

        }

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
        cMesh* pBathTub = g_pFindMeshByFriendlyName("bathtub");
        if (pBathTub)
        {
            glm::vec3 bathTubToLightRay = pBathTub->drawPosition - glm::vec3(::g_pTheLights->theLights[0].position);

            bathTubToLightRay = glm::normalize(bathTubToLightRay);

            ::g_pTheLights->theLights[0].direction = glm::vec4(bathTubToLightRay, 1.0f);
        }




        glfwSwapBuffers(window);
        glfwPollEvents();

        // Update the title screen
        std::stringstream ssTitle;
        ssTitle << "Camera (x,y,z): "
            << ::g_cameraEye.x << ", "
            << ::g_cameraEye.y << ", "
            << ::g_cameraEye.z << ") "
            << "Light[" << ::g_selectedLight << "]: "
            << ::g_pTheLights->theLights[::g_selectedLight].position.x << ", "
            << ::g_pTheLights->theLights[::g_selectedLight].position.y << ", "
            << ::g_pTheLights->theLights[::g_selectedLight].position.z << "  "
            << "const:" << ::g_pTheLights->theLights[::g_selectedLight].atten.x << " "
            << "linear:" << ::g_pTheLights->theLights[::g_selectedLight].atten.y << " "
            << "quad:" << ::g_pTheLights->theLights[::g_selectedLight].atten.z << " "
            << "inner: " << ::g_pTheLights->theLights[::g_selectedLight].param1.y << " "
            << "outer: " << ::g_pTheLights->theLights[::g_selectedLight].param1.z << " ";

        //        glfwSetWindowTitle(window, "HEY!");

        std::string theTitle = ssTitle.str();

        glfwSetWindowTitle(window, theTitle.c_str());


    }

    // Delete everything


    glfwDestroyWindow(window);




    glfwTerminate();
    exit(EXIT_SUCCESS);
}

// Returns NULL if not found
cMesh* g_pFindMeshByFriendlyName(std::string friendlyNameToFind)
{
    for (unsigned int index = 0; index != ::g_vec_pMeshesToDraw.size(); index++)
    {
        if (::g_vec_pMeshesToDraw[index]->friendlyName == friendlyNameToFind)
        {
            // Found it
            return ::g_vec_pMeshesToDraw[index];
        }
    }
    // Didn't find it
    return NULL;
}


void DrawObject(cMesh* pCurrentMesh, glm::mat4 matModelParent, GLuint shaderProgramID)
{

    //         mat4x4_identity(m);
    glm::mat4 matModel = matModelParent;




    // Translation
    glm::mat4 matTranslate = glm::translate(glm::mat4(1.0f),
        glm::vec3(pCurrentMesh->drawPosition.x,
            pCurrentMesh->drawPosition.y,
            pCurrentMesh->drawPosition.z));


    // Rotation matrix generation
//    glm::mat4 matRotateX = glm::rotate(glm::mat4(1.0f),
//                                       pCurrentMesh->drawOrientation.x, // (float)glfwGetTime(),
//                                       glm::vec3(1.0f, 0.0, 0.0f));
//
//
//    glm::mat4 matRotateY = glm::rotate(glm::mat4(1.0f),
//                                       pCurrentMesh->drawOrientation.y, // (float)glfwGetTime(),
//                                       glm::vec3(0.0f, 1.0, 0.0f));
//
//    glm::mat4 matRotateZ = glm::rotate(glm::mat4(1.0f),
//                                       pCurrentMesh->drawOrientation.z, // (float)glfwGetTime(),
//                                       glm::vec3(0.0f, 0.0, 1.0f));

    // Now we are all bougie, using quaternions
    glm::mat4 matRotation = glm::mat4(pCurrentMesh->get_qOrientation());


    // Scaling matrix
    glm::mat4 matScale = glm::scale(glm::mat4(1.0f),
        glm::vec3(pCurrentMesh->drawScale.x,
            pCurrentMesh->drawScale.y,
            pCurrentMesh->drawScale.z));
    //--------------------------------------------------------------

    // Combine all these transformation
    matModel = matModel * matTranslate;         // Done last

//    matModel = matModel * matRotateX;
//    matModel = matModel * matRotateY;
//    matModel = matModel * matRotateZ;
    //
    matModel = matModel * matRotation;


    matModel = matModel * matScale;             // Mathematically done 1st

//        m = m * rotateZ;
//        m = m * rotateY;
//        m = m * rotateZ;



   //mat4x4_mul(mvp, p, m);
//    glm::mat4 mvp = matProjection * matView * matModel;

//    GLint mvp_location = glGetUniformLocation(shaderProgramID, "MVP");
//    //glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*) mvp);
//    glUniformMatrix4fv(mvp_location, 1, GL_FALSE, glm::value_ptr(mvp));

    GLint matModel_UL = glGetUniformLocation(shaderProgramID, "matModel");
    glUniformMatrix4fv(matModel_UL, 1, GL_FALSE, glm::value_ptr(matModel));


    // Also calculate and pass the "inverse transpose" for the model matrix
    glm::mat4 matModel_InverseTranspose = glm::inverse(glm::transpose(matModel));

    // uniform mat4 matModel_IT;
    GLint matModel_IT_UL = glGetUniformLocation(shaderProgramID, "matModel_IT");
    glUniformMatrix4fv(matModel_IT_UL, 1, GL_FALSE, glm::value_ptr(matModel_InverseTranspose));


    if (pCurrentMesh->bIsWireframe)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    //        glPointSize(10.0f);


            // uniform bool bDoNotLight;
    GLint bDoNotLight_UL = glGetUniformLocation(shaderProgramID, "bDoNotLight");

    if (pCurrentMesh->bDoNotLight)
    {
        // Set uniform to true
        glUniform1f(bDoNotLight_UL, (GLfloat)GL_TRUE);
    }
    else
    {
        // Set uniform to false;
        glUniform1f(bDoNotLight_UL, (GLfloat)GL_FALSE);
    }

    //uniform bool bUseDebugColour;	
    GLint bUseDebugColour_UL = glGetUniformLocation(shaderProgramID, "bUseDebugColour");
    if (pCurrentMesh->bUseDebugColours)
    {
        glUniform1f(bUseDebugColour_UL, (GLfloat)GL_TRUE);
        //uniform vec4 debugColourRGBA;
        GLint debugColourRGBA_UL = glGetUniformLocation(shaderProgramID, "debugColourRGBA");
        glUniform4f(debugColourRGBA_UL,
            pCurrentMesh->wholeObjectDebugColourRGBA.r,
            pCurrentMesh->wholeObjectDebugColourRGBA.g,
            pCurrentMesh->wholeObjectDebugColourRGBA.b,
            pCurrentMesh->wholeObjectDebugColourRGBA.a);
    }
    else
    {
        glUniform1f(bUseDebugColour_UL, (GLfloat)GL_FALSE);
    }



    sModelDrawInfo modelInfo;
    if (::g_pMeshManager->FindDrawInfoByModelName(pCurrentMesh->meshName, modelInfo))
    {
        // Found it!!!

        glBindVertexArray(modelInfo.VAO_ID); 		//  enable VAO (and everything else)
        glDrawElements(GL_TRIANGLES,
            modelInfo.numberOfIndices,
            GL_UNSIGNED_INT,
            0);
        glBindVertexArray(0); 			            // disable VAO (and everything else)

    }

    // Are there any child meshes on this mesh
    // std::vector<cMesh*> vec_pChildMeshes;

    glm::mat4 matRemoveScaling = glm::scale(glm::mat4(1.0f),
        glm::vec3(
            1.0f / pCurrentMesh->drawScale.x,
            1.0f / pCurrentMesh->drawScale.y,
            1.0f / pCurrentMesh->drawScale.z));

    matModel = matModel * matRemoveScaling;

    for (cMesh* pChild : pCurrentMesh->vec_pChildMeshes)
    {

        // Notice we are passing the "parent" (already transformed) matrix
        // NOT an identiy matrix

        // if you are using scaling, you can "undo" the scaling
        // i.e. add the opposite of the scale the parent had
        DrawObject(pChild, matModel, shaderProgramID);

    } 


    return;
}


