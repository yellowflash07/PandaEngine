#include "Engine.h"
#include <iostream>
#include <glm/gtc/type_ptr.hpp> 
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include "GLFWCallbacks.h"
#include "ImGuizmo.h"
#include "Debug.h"

bool IMGUI_ENABLE = false;

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

Camera* camera;

//void mouse_callback(GLFWwindow* window, double xpos, double ypos)
//{
//    camera->ProcessMouseMovement(xpos, ypos);
//}

Engine::Engine()
{
	//shaderManager = new cShaderManager();
	meshManager = new MeshManager();
	lightManager = new cLightManager();
    physicsManager = new PhysicsManager(meshManager);
    audioManager = new cAudioManager(meshManager);
    camera = new Camera(glm::vec3(0.0,0.0f,0.0f),
        		        glm::vec3(0.0f, 0.0f, -1.0f),
        		        glm::vec3(0.0f, 1.0f, 0.0f), 0.1f, 10000.0f);
}

Engine::~Engine()
{
}

bool Engine::Initialize()
{
    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        return false;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);

    if (!IMGUI_ENABLE)
    {
        glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);
        glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
    }

  //  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    window = glfwCreateWindow(1920, 1080, "Template Scene", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return false;
    }

    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glfwSwapInterval(1);

  

    if (IMGUI_ENABLE)
    {
        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;        // Enable Docking

        // Setup Platform/Renderer backends
        ImGui_ImplGlfw_InitForOpenGL(window, true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
        ImGui_ImplOpenGL3_Init();
    }

    audioManager->Initialize();

    SetShaderPath("../Assets/Shaders");
    SetModelPath("../Assets/Models");
    SetAudioPath("../Assets/Audio");
    meshManager->SetTexturePath("../Assets/Textures");   

    if (!LoadDefaultShaders())
    {
        return false;
    }

    LoadDefaultLights();

    meshManager->shaderProgramID = shaderProgramID;
    assetLib.m_texManager = meshManager->GetTextureManager();
    assetLib.m_meshManager = meshManager;
    assetLib.shaderProgramID = shaderProgramID;
    assetLib.Init();
    sceneSaver.meshManager = meshManager;

    glPatchParameteri(GL_PATCH_VERTICES, 3);
    const GLubyte* version = glGetString(GL_VERSION);
    std::cout << "OPENGL VERSION: " << version << std::endl;
    return true;
}

void Engine::BeginRender()
{

    //render render textures
    for (int i = 0; i < renderTextures.size(); i++)
    {
        renderTextures[i]->Render();
    }

    float ratio;
    int width, height;

    glUseProgram(shaderProgramID);

    glfwGetFramebufferSize(window, &width, &height);
    ratio = width / (float)height;

    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (IMGUI_ENABLE)
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);

        ImGuizmo::SetOrthographic(false);
        ImGuizmo::BeginFrame();
        ImGuiIO& io = ImGui::GetIO();
        ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
	}
}

void Engine::EndRender()
{
    if (IMGUI_ENABLE)
    {
        //render the frame
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    glfwSwapBuffers(window);
    glfwPollEvents();
}

void Engine::Update()
{

    // While drawing a pixel, see if the pixel that's already there is closer or not?
    glEnable(GL_DEPTH_TEST);
   

    // Time per frame
    double currentTime = glfwGetTime();
    deltaTime = currentTime - lastTime;
    lastTime = currentTime;

    meshManager->vaoManager->CheckQueue();

    //update camera
    camera->Update(window, deltaTime);

    if (scenes.size() > 0)
    {
        scenes[currentScene]->Update((float)deltaTime);

       /* if (ImGui::Button("Save", ImVec2(100,100)))
        {
            sceneSaver.SaveScene(scenes[currentScene]);
		}*/
    }

    if (IMGUI_ENABLE)
    {
        ImGuiWindowFlags window_flags = 0;
        window_flags |= ImGuiWindowFlags_MenuBar;
        window_flags |= ImGuiWindowFlags_NoTitleBar;
        ImGui::Begin("X", &loadFile, window_flags);
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("Menu"))
            {
                if (ImGui::MenuItem("Save Scene", "Ctrl + S"))
                {
                    sceneSaver.SaveScene(scenes[currentScene], currentFile);
                }
                /* if (ImGui::MenuItem("Load Scene", NULL))
                 {
                     LoadSave();
                 }*/

                ImGui::EndMenu();
            }

            if (ImGui::Button("Play"))
            {
                scenes[currentScene]->Play();
            }
            if (ImGui::Button("Stop"))
            {
                scenes[currentScene]->Stop();
            }

            ImGui::EndMenuBar();
        }

        ImGui::End();
        //show asset library
        assetLib.RenderBox();
	}
    
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
		glfwSetWindowShouldClose(window, true);
        exit(EXIT_SUCCESS);
	}

    //update physics
    physicsManager->Update(deltaTime);

    //update audio
    audioManager->Update();

    audioManager->SetListenerAttributes(camera->cameraEye, 
                                        glm::vec3(0,0,0), 
                                        camera->GetCameraRotation() * glm::vec3(0, 0, 1),
                                        camera->upVector);

}

void Engine::SetShaderPath(std::string filePath)
{
	//shaderManager->setBasePath("../Assets/Shaders");
	cShaderManager::getInstance()->setBasePath(filePath);
}

void Engine::SetModelPath(std::string filePath)
{
    meshManager->SetBasePath(filePath);
}

void Engine::SetAudioPath(std::string filePath)
{
    audioManager->SetBasePath(filePath);
}

cMesh* Engine::LoadMesh(std::string filePath, std::string friendlyName, bool dontDraw)
{
    cMesh* mesh = nullptr;
    mesh = meshManager->AddMesh(filePath, friendlyName, shaderProgramID);
   /* if (dontDraw)
    {
		mesh = meshManager->LoadMesh(filePath, friendlyName, shaderProgramID);
	}
    else
    {
		
	}*/
    //cMesh* mesh = meshManager->AddMesh(filePath, friendlyName, shaderProgramID);
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
    vertexShader.fileName = "vertexshader.glsl";

    cShaderManager::cShader tessControlShader;
   tessControlShader.fileName = "tesscontrolshader.glsl";
    //tessControlShader.fileName = "";

    cShaderManager::cShader tessEvalShader;
    //tessEvalShader.fileName = "";
    tessEvalShader.fileName = "tessevalshader.glsl";

    cShaderManager::cShader fragmentShader;
    fragmentShader.fileName = "fragmentshader.glsl";

    if (!cShaderManager::getInstance()->createProgramFromFile("shader01", vertexShader, fragmentShader, tessControlShader, tessEvalShader))
    {
      //  std::cout << "Error: Couldn't compile or link:" << std::endl;
        std::cout << cShaderManager::getInstance()->getLastError();
        return false;
    }
    shaderProgramID = cShaderManager::getInstance()->getIDFromFriendlyName("shader01");
    std::cout << "Shader compliled! Program ID: " << shaderProgramID << std::endl;

    camera->shaderProgramID = shaderProgramID;

    return true;
}

void Engine::LoadDefaultLights()
{
    lightManager->SetUniformLocations(shaderProgramID);
    lightManager->theLights[0]->param2.x = 1; //on
    lightManager->theLights[0]->param1.x = 2; //directional
    lightManager->theLights[0]->direction = glm::vec4(-1.0,-1.0,-1.0,1); //directional
}

void Engine::LoadSave(std::string sceneName)
{
    Scene* scene = sceneSaver.LoadScene(sceneName);
    currentFile = sceneName;
   // scene->Init(this->meshManager, this->physicsManager, this->lightManager, cShaderManager::getInstance());
    AddScene(scene);
    //meshManager->LoadSavedMeshes(shaderProgramID);
	//lightManager->LoadLights();
}

void Engine::ShutDown()
{
    //delete shaderManager;
    delete meshManager;
    delete lightManager;
    delete physicsManager;
    delete audioManager;
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    //exit(EXIT_SUCCESS);
}

void Engine::AddScene(Scene* scene)
{
    scene->Init(this->meshManager, this->physicsManager, this->lightManager, cShaderManager::getInstance(), window);
    scene->window = window;
    scenes.push_back(scene);
}

RenderTexture* Engine::CreateRenderTexture(Camera* camera, std::vector<cMesh*> offScreenMesh, int width, int height)
{
    RenderTexture* rt = new RenderTexture(camera, width, height, shaderProgramID, offScreenMesh);
    rt->meshManager = this->meshManager;
    rt->lightManager = this->lightManager;
    renderTextures.push_back(rt);
    return rt;
}

Scene* Engine::GetCurrentScene()
{
    return scenes[currentScene];
}

