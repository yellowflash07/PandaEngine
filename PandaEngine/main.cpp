
#include "Engine.h"
#include "Random.h"
#include <iostream>
#include <map>
#include "CommandFactory.h"
#include <Lua5.4.6/lua.hpp>
#include <sol/sol.hpp>
#include <functional>
#include "CommandGroup.h"

extern Camera* camera;
int keyHit = 0;
int action = 0;
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS)
    {
        //   std::cout << "Hit" << key << std::endl;
        keyHit = key;
        action = action;
    }
}
float pitch = 0.0f;
float yaw = 0.0f;
float roll = 0.0f;

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    camera->ProcessMouseMovement(xpos, ypos);
    static bool firstMouse = true;
    static float lastX = 0.0f;
    static float lastY = 0.0f;

    if (firstMouse)
    {
        lastX = (float)xpos;
        lastY = (float)ypos;
        firstMouse = false;
    }


    float xoffset = (float)xpos - lastX;
    float yoffset = lastY - (float)ypos;

    lastX = (float)xpos;
    lastY = (float)ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;
}

int main(void)
{
    Engine engine;
    if (!engine.Initialize())
    {
        return 1;
    }

#pragma region Skybox
     engine.meshManager->LoadTexture("PaletteV1.bmp");
    bool loaded = engine.meshManager->LoadCubeMap("space",
                                    "CubeMaps/TropicalSunnyDayLeft2048.bmp",
                                    "CubeMaps/TropicalSunnyDayRight2048.bmp",
                                    "CubeMaps/TropicalSunnyDayUp2048.bmp",
                                    "CubeMaps/TropicalSunnyDayDown2048.bmp",
                                    "CubeMaps/TropicalSunnyDayFront2048.bmp",
                                    "CubeMaps/TropicalSunnyDayBack2048.bmp",
                                    true);
    
    cMesh* skyBoxMesh = engine.LoadMesh("Sphere_1_unit_Radius_UV.ply", "skybox");
    skyBoxMesh->isSkyBox = true;
    skyBoxMesh->setUniformDrawScale(5000.0f);

#pragma endregion

    sol::state lua;

 
	cMesh* sphere = engine.LoadMesh("Sphere_1_unit_Radius_UV.ply", "sphere");
    sphere->bDoNotLight = true;
    sphere->bUseDebugColours = true;
    sphere->bIsVisible = false;
    sphere->drawPosition = glm::vec3(0, 0, 100);

    cMesh* mesh = engine.LoadMesh("bathtub_xyz_n_rgba.ply", "mesh");
 
    mesh->bUseDebugColours = true;
    mesh->wholeObjectDebugColourRGBA = glm::vec4(1, 0, 0, 1);

 
    lua.script_file("MoveTo.lua");  
 
   sol::table myFunctions = lua["commandGroups"];
   CommandFactory commandFactory;
   CommandGroup scene1("command");

   

   for (auto& entry : myFunctions) {

       std::string identifier = entry.first.as<std::string>();
       sol::table commandGroup = entry.second;


   		std::cout << "identifier: " << identifier << std::endl;
   		std::string commandNameValue = commandGroup[5];
       
 

       if(commandNameValue == "MoveTo")

       {
       	std::string mesh = commandGroup[1];
        std::string sphere = commandGroup[2];
	    MoveTo* moveTo = (MoveTo*)commandFactory.CreateCommand("MoveTo","MoveTofriendlyName");
        cMesh* sphereMesh = engine.meshManager->FindMeshByFriendlyName(sphere);
        cMesh* meshMesh = engine.meshManager->FindMeshByFriendlyName(mesh);
        int speed = commandGroup[3];
		moveTo->SetParams(meshMesh, sphereMesh, speed);
        scene1.AddSerialCommand(moveTo);
       }
	   else if (commandNameValue == "OrientTo")
	   {
        std::string mesh = commandGroup[1];
        std::string sphere = commandGroup[2];
 		OrientTo* orientTo = (OrientTo*)commandFactory.CreateCommand("OrientTo", "OrientTofriendlyName"); 
        cMesh* sphereMesh = engine.meshManager->FindMeshByFriendlyName(sphere);
	   	cMesh* meshMesh = engine.meshManager->FindMeshByFriendlyName(mesh);
        int speed = commandGroup[3];
        int time = commandGroup[4];
        orientTo->SetParams(meshMesh, sphereMesh, speed,time);

	   }
       else if (commandNameValue == "FollowObject")
       {
	    std::string mesh = commandGroup[1];
        std::string sphere = commandGroup[2];
 		FollowObject* followObject = (FollowObject*)commandFactory.CreateCommand("FollowObject", "FollowObjectfriendlyName");
        cMesh* sphereMesh = engine.meshManager->FindMeshByFriendlyName(sphere);
        cMesh* meshMesh = engine.meshManager->FindMeshByFriendlyName(mesh);

		int minSpeed = commandGroup[4];
		int maxSpeed = commandGroup[4];
        //sol::table vectorTable = commandGroup[3]; 
       /*	float x = vectorTable[1];
        float y = vectorTable[2];
        float z = vectorTable[3];
        glm::vec3 offset(x, y, z);*/

        glm::vec3 offset(
            commandGroup[3][1].get<float>(),
            commandGroup[3][2].get<float>(),
            commandGroup[3][3].get<float>()
        );

 
		followObject->SetParams(meshMesh, sphereMesh, offset, minSpeed, maxSpeed);
	   }
	   else if (commandNameValue == "FollowCurve")
	   {
         std::string mesh = commandGroup[1];
         FollowCurve* followCurve = (FollowCurve*)commandFactory.CreateCommand("FollowCurve", "FollowCurvefriendlyName");
       cMesh* meshMesh = engine.meshManager->FindMeshByFriendlyName(mesh);

   
        
       sol::table followCurveTable = commandGroup[4];
       sol::table curveTable = followCurveTable[3];

     
       glm::vec3 curvePoint1(
           curveTable[1][1].get<float>(),
           curveTable[1][2].get<float>(),
           curveTable[1][3].get<float>()
       );

       glm::vec3 curvePoint2(
           curveTable[2][1].get<float>(),
           curveTable[2][2].get<float>(),
           curveTable[2][3].get<float>()
       );

       glm::vec3 curvePoint3(
           curveTable[3][1].get<float>(),
           curveTable[3][2].get<float>(),
           curveTable[3][3].get<float>()
       );

       std::vector<glm::vec3> curvePoints;
       curvePoints.push_back(curvePoint1);
       curvePoints.push_back(curvePoint2);
       curvePoints.push_back(curvePoint3);

       followCurve->SetParams(meshMesh, curvePoints, true);

       }

       else if (commandNameValue == "LocationTrigger")
       {
           LocationTrigger* locationTrigger = (LocationTrigger*)commandFactory.CreateCommand("LocationTrigger", "LocationTriggerfriendlyName");
           std::string mesh = commandGroup[1];
           glm::vec3 locationOffset(
               commandGroup[2][1].get<float>(),
               commandGroup[2][2].get<float>(),
               commandGroup[2][3].get<float>()
           );
           float size = commandGroup[3];

           //locationTrigger->SetParams(engine.meshManager->FindMeshByFriendlyName(mesh)->pAABB, locationOffset, size);
       }
       else if (commandNameValue == "LightControl")
       {
			LightControl* lightControl = (LightControl*)commandFactory.CreateCommand("LightControl", "LightControlfriendlyName");
           int lightNumber = commandGroup[1];
           glm::vec3 locationOffset(
               commandGroup[2][1].get<float>(),
               commandGroup[2][2].get<float>(),
               commandGroup[2][3].get<float>()
           );
           int speed = commandGroup[3];

        //engine.lightManager->[lightNumber]->SetParams(locationOffset, speed);

       }
       else if (commandNameValue == "ScaleUp")
       {
           std::string mesh = commandGroup[1];
        float speed = commandGroup[2];
        float scale = commandGroup[3];
         
        ScaleUp* scaleUp = (ScaleUp*)commandFactory.CreateCommand("ScaleUp", "ScaleUpfriendlyName");
        scaleUp->SetParams(engine.meshManager->FindMeshByFriendlyName(mesh), speed, scale);
       }
 
       //// Print the extracted values
       //std::cout << "mesh: " << mesh << std::endl;
       //std::cout << "sphere: " << sphere << std::endl;
       //std::cout << "value1: " << value1 << std::endl;
       //std::cout << "value2: " << value2 << std::endl;
   }

 
 
 
  engine.LoadDefaultLights();

    float currTime = 0;
    float myTime = 0;
    while (!glfwWindowShouldClose(engine.window))
    {
        engine.Update();
        skyBoxMesh->drawPosition = camera->cameraEye;

        currTime += engine.deltaTime;

        if (glfwGetKey(engine.window, GLFW_KEY_1) == GLFW_PRESS)
        {
            
			scene1.Execute(engine.deltaTime);
            
        }
    }

    engine.ShutDown();
}
