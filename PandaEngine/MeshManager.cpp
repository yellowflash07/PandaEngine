#include "MeshManager.h"
#include <iostream>
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr
#include <imgui.h>


MeshManager::MeshManager()
{
   vaoManager = new cVAOManager();
   saver = new SceneSaver();
}

MeshManager::~MeshManager()
{

}

cMesh* MeshManager::AddMesh(std::string modelNameAtPath, std::string friendlyName, unsigned int shaderProgramID)
{
    sModelDrawInfo drawInfo;

    if (!vaoManager->LoadModelIntoVAOAI(modelNameAtPath, drawInfo, shaderProgramID))
    {
		//  DoTheErrorLogging("Didn't load model");
		return nullptr;
	}

    cMesh* mesh = new cMesh();
    mesh->meshName = modelNameAtPath;
    mesh->friendlyName = friendlyName;
    std::cout << "Loaded: " << drawInfo.numberOfVertices << " vertices" << std::endl;
    meshList.push_back(mesh);
    return mesh;
}

void MeshManager::DrawObject(cMesh* pCurrentMesh, glm::mat4 matModelParent, GLuint shaderProgramID)
{
    glm::mat4 matModel = matModelParent;

    // Translation
    glm::mat4 matTranslate = glm::translate(glm::mat4(1.0f),
        glm::vec3(pCurrentMesh->drawPosition.x,
            pCurrentMesh->drawPosition.y,
            pCurrentMesh->drawPosition.z));

    //rotation
    glm::mat4 matRotation = glm::mat4(pCurrentMesh->get_qOrientation());


    // Scaling matrix
    glm::mat4 matScale = glm::scale(glm::mat4(1.0f),
        glm::vec3(pCurrentMesh->drawScale.x,
            pCurrentMesh->drawScale.y,
            pCurrentMesh->drawScale.z));
    //--------------------------------------------------------------

    // Combine all these transformation
    matModel = matModel * matTranslate;         // Done last

    matModel = matModel * matRotation;

    matModel = matModel * matScale;

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

    GLint bDoNotLight_UL = glGetUniformLocation(shaderProgramID, "bDoNotLight");

    if (pCurrentMesh->bDoNotLight)
    {
        glUniform1f(bDoNotLight_UL, (GLfloat)GL_TRUE);
    }
    else
    {
        glUniform1f(bDoNotLight_UL, (GLfloat)GL_FALSE);
    }

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

    GLint colour_UL = glGetUniformLocation(shaderProgramID, "color");
    glUniform4f(colour_UL,
        pCurrentMesh->color.r,
        pCurrentMesh->color.g,
        pCurrentMesh->color.b,
        pCurrentMesh->color.a);

    sModelDrawInfo modelInfo;
    if (vaoManager->FindDrawInfoByModelName(pCurrentMesh->meshName, modelInfo))
    {
        // Found it!!!

        glBindVertexArray(modelInfo.VAO_ID); 		//  enable VAO (and everything else)
        glDrawElements(GL_TRIANGLES,
            modelInfo.numberOfIndices,
            GL_UNSIGNED_INT,
            0);
        glBindVertexArray(0); 			            // disable VAO (and everything else)

    }


    glm::mat4 matRemoveScaling = glm::scale(glm::mat4(1.0f),
        glm::vec3(
            1.0f / pCurrentMesh->drawScale.x,
            1.0f / pCurrentMesh->drawScale.y,
            1.0f / pCurrentMesh->drawScale.z));

    matModel = matModel * matRemoveScaling;

    for (cMesh* pChild : pCurrentMesh->vec_pChildMeshes)
    {

        DrawObject(pChild, matModel, shaderProgramID);

    }

    return;
}

void MeshManager::DrawAllObjects(GLuint shaderProgramID)
{
    ImGui::Begin("Meshes");

    for (unsigned int index = 0; index != meshList.size(); index++)
    {
        cMesh* pCurrentMesh = meshList[index];

        if (ImGui::Button(pCurrentMesh->friendlyName.c_str(), ImVec2(100,25)))
        {
            selectedMesh = pCurrentMesh;        
        }

        if (pCurrentMesh->bIsVisible)
        {
            glm::mat4 matModel = glm::mat4(1.0f);   // Identity matrix
            DrawObject(pCurrentMesh, matModel, shaderProgramID);
        }
    }
    ImGui::End();
    DrawTransformBox();
}

void MeshManager::SetBasePath(std::string basePath)
{
    vaoManager->setBasePath(basePath);
    this->basePath = basePath;
}

cMesh* MeshManager::FindMeshByFriendlyName(std::string friendlyNameToFind)
{
    for (unsigned int index = 0; index != meshList.size(); index++)
    {
        if (meshList[index]->friendlyName == friendlyNameToFind)
        {
            // Found it
            return meshList[index];
        }
    }
    // Didn't find it
    return NULL;
}

void MeshManager::DrawTransformBox()
{
    if (selectedMesh == nullptr) return;
    std::string boxName = "Transform " + selectedMesh->friendlyName;
    ImGui::Begin(boxName.c_str());

    ImGui::Text("Position"); ImGui::SetNextItemWidth(40);
    ImGui::InputFloat("xP", &selectedMesh->drawPosition.x); ImGui::SameLine(); ImGui::SetNextItemWidth(40);
    ImGui::InputFloat("yP", &selectedMesh->drawPosition.y); ImGui::SameLine(); ImGui::SetNextItemWidth(40);
    ImGui::InputFloat("zP", &selectedMesh->drawPosition.z); 

    ImGui::Text("Rotation"); ImGui::SetNextItemWidth(40);
    glm::vec3 euler = selectedMesh->eulerRotation;
    ImGui::InputFloat("xR", &euler.x); ImGui::SameLine(); ImGui::SetNextItemWidth(40);
    ImGui::InputFloat("yR", &euler.y); ImGui::SameLine(); ImGui::SetNextItemWidth(40);
    ImGui::InputFloat("zR", &euler.z);
    selectedMesh->setRotationFromEuler(euler);

    ImGui::Text("Scale"); ImGui::SetNextItemWidth(40);
    ImGui::InputFloat("xS", &selectedMesh->drawScale.x); ImGui::SameLine(); ImGui::SetNextItemWidth(40);
    ImGui::InputFloat("yS", &selectedMesh->drawScale.y); ImGui::SameLine(); ImGui::SetNextItemWidth(40);
    ImGui::InputFloat("zS", &selectedMesh->drawScale.z); ImGui::SetNextItemWidth(100);

    if (ImGui::Button("Save"))
    {
        saver->SaveMeshes(meshList);
    }
    ImGui::End();
}

void MeshManager::LoadSavedMeshes(unsigned int shaderProgramID)
{
    std::vector<cMesh*> meshes = saver->LoadMeshes();
    for (size_t i = 0; i < meshes.size(); i++)
    {
        cMesh* mesh = AddMesh(meshes[i]->meshName, meshes[i]->friendlyName, shaderProgramID);
        mesh->drawPosition = meshes[i]->drawPosition;
        mesh->setRotationFromEuler(meshes[i]->eulerRotation);
        mesh->drawScale = meshes[i]->drawScale;
        mesh->color = meshes[i]->color;
    }
}

bool MeshManager::GetModelDrawInfo(std::string friendlyName, sModelDrawInfo& drawInfo)
{  
    return vaoManager->FindDrawInfoByModelName(FindMeshByFriendlyName(friendlyName)->meshName, drawInfo);
}

void MeshManager::ToggleWireframe(bool wireframe)
{
    for (size_t i = 0; i < meshList.size(); i++)
    {
        meshList[i]->bIsWireframe = wireframe;
    }
}

