#include "Engine.h"
#include <iostream>
#include <glm/gtc/type_ptr.hpp> 
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include "GLFWCallbacks.h"

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}



Engine::Engine()
{
	shaderManager = new cShaderManager();
	meshManager = new MeshManager();
	lightManager = new cLightManager();
    physicsManager = new PhysicsManager(meshManager);
    cameraEye = glm::vec3(0.0, 70.0, 181.0f);
    cameraTarget = glm::vec3(0.0f, 5.0f, 0.0f);
    upVector = glm::vec3(0.0f, 1.0f, 0.0f);
    shaderProgramID = 1232;
    lastTime = glfwGetTime();
    near = 0.1f;
    far = 1000.0f;
}

Engine::~Engine()
{
}

bool Engine::Initialize()
{
    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        return false;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    window = glfwCreateWindow(640, 480, "Template Scene", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return false;
    }

    glfwSetKeyCallback(window, key_callback);
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glfwSwapInterval(1);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
    ImGui_ImplOpenGL3_Init();

    return true;
}

void Engine::Update()
{
    float ratio;
    int width, height;

    glUseProgram(shaderProgramID);

    glfwGetFramebufferSize(window, &width, &height);
    ratio = width / (float)height;

    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();


    // While drawing a pixel, see if the pixel that's already there is closer or not?
    glEnable(GL_DEPTH_TEST);
    // (Usually) the default - does NOT draw "back facing" triangles
    glCullFace(GL_BACK);

    lightManager->UpdateUniformValues(shaderProgramID);

    ImGui::Begin("Camera Controls");

    if (ImGui::SliderFloat("Camera X", &cameraEye.x, -1000, 1000))
    {

    }
    if (ImGui::SliderFloat("Camera Y", &cameraEye.y, -1000, 1000))
    {

    }
    if (ImGui::SliderFloat("Camera Z", &cameraEye.z, -1000, 1000))
    {

    }

    if (ImGui::SliderFloat("Camera Target X", &cameraTarget.x, -1000, 1000))
    {

    }
    if (ImGui::SliderFloat("Camera Target Y", &cameraTarget.y, -1000, 1000))
    {

    }
    if (ImGui::SliderFloat("Camera Target Z", &cameraTarget.z, -1000, 1000))
    {

    }

    ImGui::End();

    GLint eyeLocation_UL = glGetUniformLocation(shaderProgramID, "eyeLocation");
    glUniform4f(eyeLocation_UL,
        cameraEye.x, cameraEye.y, cameraEye.z, 1.0f);

    glm::mat4 matProjection = glm::perspective(0.6f,
        ratio,
        near,
        far);

    glm::mat4 matView = glm::lookAt(cameraEye,
        cameraTarget,
        upVector);

    GLint matProjection_UL = glGetUniformLocation(shaderProgramID, "matProjection");
    glUniformMatrix4fv(matProjection_UL, 1, GL_FALSE, glm::value_ptr(matProjection));

    GLint matView_UL = glGetUniformLocation(shaderProgramID, "matView");
    glUniformMatrix4fv(matView_UL, 1, GL_FALSE, glm::value_ptr(matView));


    // *********************************************************************       
    meshManager->DrawAllObjects(shaderProgramID);

    // Time per frame (more or less)
    double currentTime = glfwGetTime();
    deltaTime = currentTime - lastTime;
    lastTime = currentTime;

    physicsManager->Update(deltaTime);

    //render the frame
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


    glfwSwapBuffers(window);
    glfwPollEvents();

}

void Engine::SetShaderPath(std::string filePath)
{
	//shaderManager->setBasePath("../Assets/Shaders");
	shaderManager->setBasePath(filePath);
}

void Engine::SetModelPath(std::string filePath)
{
    meshManager->SetBasePath(filePath);
}

cMesh* Engine::LoadMesh(std::string filePath, std::string friendlyName)
{
    cMesh* mesh = meshManager->AddMesh(filePath, friendlyName, shaderProgramID);
    return mesh;
}

PhysicsBody* Engine::AddPhysicsBody(std::string friendlyMeshName)
{
    cMesh* mesh = meshManager->FindMeshByFriendlyName(friendlyMeshName);
    PhysicsBody* pb = new PhysicsBody();
    pb->mesh = mesh;
    physicsManager->AddMesh(pb);
    return pb;
}


bool Engine::LoadDefaultShaders()
{
    cShaderManager::cShader vertexShader;
    vertexShader.fileName = "vertexShader.glsl";

    cShaderManager::cShader fragmentShader;
    fragmentShader.fileName = "fragmentShader.glsl";

    if (!shaderManager->createProgramFromFile("shader01", vertexShader, fragmentShader))
    {
        std::cout << "Error: Couldn't compile or link:" << std::endl;
        std::cout << shaderManager->getLastError();
        return false;
    }
    shaderProgramID = shaderManager->getIDFromFriendlyName("shader01");
    std::cout << "Shader compliled! Program ID: " << shaderProgramID << std::endl;

 
   // meshManager->LoadSavedMeshes(shaderProgramID);
  //  lightManager->LoadLights();

    return true;
}

void Engine::LoadDefaultLights()
{
    lightManager->SetUniformLocations(shaderProgramID);
    lightManager->theLights[0]->param2.x = 1; //on
    lightManager->theLights[0]->param1.x = 2; //directional
    lightManager->theLights[0]->direction = glm::vec4(-1.0,1.0,-1.0,1); //directional
}

void Engine::LoadSave()
{
    meshManager->LoadSavedMeshes(shaderProgramID);
	//lightManager->LoadLights();
}

void Engine::ShutDown()
{
    delete shaderManager;
    delete meshManager;
    delete lightManager;
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    //exit(EXIT_SUCCESS);
}

void Engine::SetCameraDefaults(glm::vec3 position, glm::vec3 target, glm::vec3 upVector, float far, float near)
{
    cameraEye = position;
	cameraTarget = target;
	this->upVector = upVector;
    this->near = near;
    this->far = far;
}

void Engine::SetCameraTarget(glm::vec3 target)
{
    cameraTarget = target;
}
