#include "MeshManager.h"
#include <iostream>
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr
#include <imgui.h>
#include "Camera.h"
#include "ImGuizmo.h"
#include <glm/gtc/type_ptr.hpp> 
#include <glm/gtx/matrix_decompose.hpp>

extern Camera* camera;

MeshManager::MeshManager()
{
   vaoManager = new cVAOManager();
   textureManager = new cBasicTextureManager();
   saver = new SceneSaver();
}

MeshManager::~MeshManager()
{
	delete vaoManager;
	delete textureManager;
	delete saver;
}

cMesh* MeshManager::AddMesh(std::string modelNameAtPath, std::string friendlyName, unsigned int shaderProgramID)
{
    sModelDrawInfo drawInfo;
    this->shaderProgramID = shaderProgramID;
    if (!vaoManager->LoadModelIntoVAOAI(modelNameAtPath, drawInfo, shaderProgramID))
    {
		//  DoTheErrorLogging("Didn't load model");
		return nullptr;
	}

    cMesh* mesh = new cMesh();
    mesh->meshName = modelNameAtPath;
    mesh->modelDrawInfo = drawInfo;
    mesh->friendlyName = friendlyName;

    for (size_t i = 0; i < meshList.size(); i++)
    {
        if (meshList[i]->friendlyName == friendlyName)
        {
            mesh->friendlyName = friendlyName + std::to_string(i);
        }
    }

    std::cout << "Loaded: " << drawInfo.numberOfVertices << " vertices" << std::endl;
    meshList.push_back(mesh);

    for (int j = 0; j < 150; ++j)
    {
        glm::mat4 boneMatrix = glm::mat4(1.0f);
        std::string boneUL = "BoneMatrices[" + std::to_string(j) + "]";
        GLint boneUL_ID = glGetUniformLocation(shaderProgramID, boneUL.c_str());
        glUniformMatrix4fv(boneUL_ID, 1, GL_FALSE, glm::value_ptr(boneMatrix));
    }

    return mesh;
}

cMesh* MeshManager::LoadMesh(std::string modelNameAtPath, std::string friendlyName, unsigned int shaderProgramID)
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
    mesh->modelDrawInfo = drawInfo;
    std::cout << "Loaded: " << drawInfo.numberOfVertices << " vertices" << std::endl;
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


 //   // Also calculate and pass the "inverse transpose" for the model matrix
    glm::mat4 matModel_InverseTranspose = glm::inverse(glm::transpose(matModel));

 //   // uniform mat4 matModel_IT;
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

    GLint bhasVertexColors_UL = glGetUniformLocation(shaderProgramID, "hasVertexColor");
    if (pCurrentMesh->hasVertexColors)
    {
		glUniform1f(bhasVertexColors_UL, (GLfloat)GL_TRUE);
        GLint colour_UL = glGetUniformLocation(shaderProgramID, "color");
        glUniform4f(colour_UL,
            pCurrentMesh->color.r,
            pCurrentMesh->color.g,
            pCurrentMesh->color.b,
            pCurrentMesh->color.a);
	}
    else
    {
		glUniform1f(bhasVertexColors_UL, (GLfloat)GL_FALSE);
	}


 // 

    GLint bIsSkyBox_UL = glGetUniformLocation(shaderProgramID, "bIsSkyBox");
    if (pCurrentMesh->isSkyBox)
    {
        glUniform1f(bIsSkyBox_UL, (GLfloat)GL_TRUE);
        glCullFace(GL_FRONT);
    }
    else
    {
		glUniform1f(bIsSkyBox_UL, (GLfloat)GL_FALSE);
        glCullFace(GL_BACK);
	}

    GLint uvoffset_ul = glGetUniformLocation(shaderProgramID, "UV_Offset");

    glUniform2f(uvoffset_ul, pCurrentMesh->UV_Offset.x, pCurrentMesh->UV_Offset.y);

    GLint isReflective_UL = glGetUniformLocation(shaderProgramID, "IsReflective");
    if (pCurrentMesh->isReflective)
    {
		glUniform1f(isReflective_UL, (GLfloat)GL_TRUE);
	}
    else
    {
		glUniform1f(isReflective_UL, (GLfloat)GL_FALSE);
	}

	SetUpTextures(pCurrentMesh, shaderProgramID);

    if (pCurrentMesh->transperancy < 1.0f)
    {
        glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
    else
    {
		glDisable(GL_BLEND);
	}
    GLint useBone_UL = glGetUniformLocation(shaderProgramID, "useBones");
    glUniform1f(useBone_UL, pCurrentMesh->useBone ? (GLfloat)GL_TRUE : (GLfloat)GL_FALSE);

    for (int j = 0; j < 150; ++j)
    {
        glm::mat4 boneMatrix = glm::mat4(1.0f);
        std::string boneUL = "BoneMatrices[" + std::to_string(j) + "]";
        GLint boneUL_ID = glGetUniformLocation(shaderProgramID, boneUL.c_str());
        glUniformMatrix4fv(boneUL_ID, 1, GL_FALSE, glm::value_ptr(boneMatrix));
    }

    //uniform float transparency;
    GLint transparency_UL = glGetUniformLocation(shaderProgramID, "transparency");
    glUniform1f(transparency_UL, pCurrentMesh->transperancy);

    sModelDrawInfo modelInfo;
    if (vaoManager->FindDrawInfoByModelName(pCurrentMesh->meshName, modelInfo))
    {
        if (pCurrentMesh->useBone)
        {
            CalculateMatrices(pCurrentMesh, modelInfo.RootNode, glm::mat4(1.0f), modelInfo);
            //printf("----------------\n");
            for (int j = 0; j < modelInfo.vecBoneInfo.size(); ++j)
            {
                glm::mat4 boneMatrix = modelInfo.vecBoneInfo[j].FinalTransformation;
                std::string boneUL = "BoneMatrices[" + std::to_string(j) + "]";
                GLint boneUL_ID = glGetUniformLocation(shaderProgramID, boneUL.c_str());
                glUniformMatrix4fv(boneUL_ID, 1, GL_FALSE, glm::value_ptr(boneMatrix));                
            }           
        }        

        // Found it!!!
        if (!pCurrentMesh->hideParent)
        {
            glBindVertexArray(modelInfo.VAO_ID); 		//  enable VAO (and everything else)
            glDrawElements(GL_TRIANGLES,
                modelInfo.numberOfIndices,
                GL_UNSIGNED_INT,
                0);
            glBindVertexArray(0);
           // return;
        }
		            // disable VAO (and everything else)

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
    ImGui::SetNextWindowContentSize(ImVec2(500, 500));
    ImGui::Text("Drag a model here");
    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Model_DND"))
        {
            const char* payload_n = (const char*)payload->Data;
            AddMesh(payload_n, payload_n, shaderProgramID);
            //std::cout << "Accepted: " << payload_n << std::endl;
        }
        ImGui::EndDragDropTarget();
    }

    for (unsigned int index = 0; index != meshList.size(); index++)
    {
        cMesh* pCurrentMesh = meshList[index];
        if (ImGui::Button(pCurrentMesh->friendlyName.c_str(), ImVec2(100, 25)))
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

void MeshManager::SetTexturePath(std::string texturePath)
{
    textureManager->SetBasePath(texturePath);
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
    selectedMesh->bIsWireframe = false;
   
    ImGui::Begin(boxName.c_str());
    std::string friendName = selectedMesh->friendlyName;
    if (ImGui::InputText("Name", &friendName[0], 100, ImGuiInputTextFlags_::ImGuiInputTextFlags_EnterReturnsTrue))
    {
		selectedMesh->friendlyName = friendName.c_str();
	}

    ImGui::SetNextWindowContentSize(ImVec2(250, 250));
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

    ImGui::SliderFloat("Transparency", &selectedMesh->transperancy, 0.0f, 1.0f); ImGui::SameLine();

    glm::mat4 meshTransform = selectedMesh->GetTransform();

    static ImGuizmo::OPERATION gizmoOperation(ImGuizmo::OPERATION::UNIVERSAL);
    bool isTranslateRadio = false;
    ImGui::NewLine();
    if(ImGui::RadioButton("Translate", &isTranslateRadio))
    {
		gizmoOperation = ImGuizmo::OPERATION::TRANSLATE;
    }
    bool isRotateRadio = false;
    ImGui::SameLine();
    if (ImGui::RadioButton("Rotate", &isRotateRadio))
    {
        gizmoOperation = ImGuizmo::OPERATION::ROTATE;
    }
    bool isScaleRadio = false;
    ImGui::SameLine();
    if (ImGui::RadioButton("Scale", &isScaleRadio))
    {
        gizmoOperation = ImGuizmo::OPERATION::SCALE;
    }


    ImGui::SameLine();
    ImGuizmo::Manipulate(glm::value_ptr(camera->matView),
                        glm::value_ptr(camera->matProjection),
                        gizmoOperation,
                        ImGuizmo::MODE::WORLD,
                        glm::value_ptr(meshTransform));

    if(ImGuizmo::IsUsing())
	{
		glm::vec3 position, scale;
		glm::quat rotation;
		glm::vec3 skew;
		glm::vec4 perspective;
		glm::decompose(meshTransform, scale, rotation, position, skew, perspective);
		selectedMesh->drawPosition = position;
		selectedMesh->drawScale = scale;
		selectedMesh->setRotationFromQuat(rotation);
	}

    ImGui::NewLine();
    for (size_t i = 0; i < selectedMesh->NUM_OF_TEXTURES; i++)
    {
        if (selectedMesh->texture[i].empty())
        {
            ImGui::Text("Texture Slot: %d EMPTY", i);
            if (ImGui::BeginDragDropTarget())
            {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Texture_DND"))
                {
                    const char* payload_n = (const char*)payload->Data;
                    selectedMesh->texture[i] = payload_n;
                    selectedMesh->textureRatio[i] = 1.0f;
                    selectedMesh->hasVertexColors = false;
                    selectedMesh->bUseDebugColours = false;
                }
                ImGui::EndDragDropTarget();
            }
           // break;
        }
        else
        {
            ImGui::Text("Texture Slot: %s", selectedMesh->texture[i].c_str());
            if(ImGui::SliderFloat("Mix Ratio", &selectedMesh->textureRatio[i], 0.0f, 1.0f))
            {
            }
            if (ImGui::Button("Remove Texture"))
            {
				selectedMesh->texture[i] = "";
				selectedMesh->textureRatio[i] = 0.0f;
			}
        }
    }

    ImGui::NewLine();
    ImGui::Text("Mask Texture: %s", selectedMesh->maskTexture.c_str());
    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Texture_DND"))
        {
			const char* payload_n = (const char*)payload->Data;
			selectedMesh->maskTexture = payload_n;
		}
		ImGui::EndDragDropTarget();
	}

    if (!selectedMesh->maskTexture.empty())
    {
        if (ImGui::Button("Remove Mask"))
        {
            selectedMesh->maskTexture = "";
        }
    }

    if (ImGui::Button("Save"))
    {
        saver->SaveMeshes(meshList);
    }
    ImGui::SameLine();
    if (ImGui::Button("Delete"))
    {
		RemoveMesh(selectedMesh->friendlyName);
        selectedMesh = nullptr;
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
        mesh->transperancy = meshes[i]->transperancy;
        mesh->maskTexture = meshes[i]->maskTexture;
        for (size_t j = 0; j < cMesh::NUM_OF_TEXTURES; j++)
        {
			mesh->texture[j] = meshes[i]->texture[j];
			mesh->textureRatio[j] = meshes[i]->textureRatio[j];
		}
    }
}

bool MeshManager::GetModelDrawInfo(std::string friendlyName, sModelDrawInfo& drawInfo)
{  
    return vaoManager->FindDrawInfoByModelName(FindMeshByFriendlyName(friendlyName)->meshName, drawInfo);
}

bool MeshManager::GetTransformedMeshDrawInfo(std::string friendlyName, sModelDrawInfo& drawInfo)
{
 //   sModelDrawInfo modelInfo;
    if (!GetModelDrawInfo(friendlyName, drawInfo))
    {
		return false;
	}

    cMesh* pCurrentMesh = FindMeshByFriendlyName(friendlyName);

    glm::mat4 matModel = glm::mat4(1.0f);   // Identity matrix
    glm::mat4 matTranslate = glm::translate(glm::mat4(1.0f),
        		                glm::vec3(pCurrentMesh->drawPosition.x,
                    			pCurrentMesh->drawPosition.y,
                    			pCurrentMesh->drawPosition.z));

    glm::mat4 matRotation = glm::mat4(pCurrentMesh->get_qOrientation());

    glm::mat4 matScale = glm::scale(glm::mat4(1.0f),
        		                glm::vec3(pCurrentMesh->drawScale.x,
                    			pCurrentMesh->drawScale.y,
                    			pCurrentMesh->drawScale.z));

    matModel = matModel * matTranslate;         // Done last
    matModel = matModel * matRotation;
    matModel = matModel * matScale;

    for (size_t i = 0; i < drawInfo.numberOfVertices; i++)
    {
        glm::vec3 vert = glm::vec3(drawInfo.pVertices[i].x,
            drawInfo.pVertices[i].y,
            drawInfo.pVertices[i].z);
        vert = (matModel * glm::vec4(vert, 1.0f));

        drawInfo.pVertices[i].x = vert.x;
        drawInfo.pVertices[i].y = vert.y;
        drawInfo.pVertices[i].z = vert.z;
    }  

    return true;
}

void MeshManager::ToggleWireframe(bool wireframe)
{
    for (size_t i = 0; i < meshList.size(); i++)
    {
        meshList[i]->bIsWireframe = wireframe;
    }
}

bool MeshManager::LoadTexture(std::string textureFileName)
{
    if (textureManager->Create2DTextureFromBMPFile(textureFileName, true))
    {
        std::cout << "Loaded texture: " << textureFileName << std::endl;
        return true;
    }
    std::cout << "Didn't load texture: " << textureFileName << std::endl;
    return false;
}

bool MeshManager::LoadCubeMap(std::string cubeMapName, std::string posX_fileName, std::string negX_fileName, std::string posY_fileName, std::string negY_fileName, std::string posZ_fileName, std::string negZ_fileName, bool bIsSeamless)
{
    std::string errorString;
    return textureManager->CreateCubeTextureFromBMPFiles(cubeMapName, 
        posX_fileName, negX_fileName, 
        posY_fileName, negY_fileName,
        posZ_fileName, negZ_fileName,
        bIsSeamless, errorString);

}

void MeshManager::RemoveMesh(std::string friendlyName)
{
    for (size_t i = 0; i < meshList.size(); i++)
    {
        cMesh* mesh = meshList[i];
        if (mesh->friendlyName == friendlyName)
        {
			meshList.erase(meshList.begin() + i);
           // delete mesh;
			return;
		}
	}
}

void MeshManager::UpdateVAOBuffers(std::string friendlyName, sModelDrawInfo& drawInfo)
{    
    vaoManager->UpdateVAOBuffers(friendlyName, drawInfo, shaderProgramID);
}


void MeshManager::SetUpTextures(cMesh* pCurrentMesh, GLuint shaderProgramID)
{
    if (pCurrentMesh->isSkyBox)
    {
        GLint textureUnit30 = 30;
        GLuint skyBoxID = textureManager->getTextureIDFromName("space");
        glActiveTexture(GL_TEXTURE0 + textureUnit30);
        // NOTE: Binding is NOT to GL_TEXTURE_2D
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyBoxID);
        GLint skyBoxSampler_UL = glGetUniformLocation(shaderProgramID, "skyBoxCubeMap");
        glUniform1i(skyBoxSampler_UL, textureUnit30);
        return;
    }

    if (pCurrentMesh->renderTextureID > 0)
    {
        //GLint renderTextureBool_UL = glGetUniformLocation(shaderProgramID, "hasRenderTexture");
        //glUniform1f(renderTextureBool_UL, (GLfloat)GL_TRUE);
        GLint textureBool_UL = glGetUniformLocation(shaderProgramID, "hasRenderTexture");
        glUniform1f(textureBool_UL, (GLfloat)GL_TRUE);

        GLint textureUnitNumber = 0;
        glActiveTexture(GL_TEXTURE0 + textureUnitNumber);
        glBindTexture(GL_TEXTURE_2D, pCurrentMesh->renderTextureID);
        GLint texture_00_UL = glGetUniformLocation(shaderProgramID, "renderTexture");
        glUniform1i(texture_00_UL, textureUnitNumber);

  /*      GLint textureMixRatio_0_3_UL = glGetUniformLocation(shaderProgramID, "textureMixRatio_0_3");

        glUniform4f(textureMixRatio_0_3_UL,
            1.0f,
            1.0f,
            1.0f,
            1.0f);*/

        return;
	}

    GLint renderTextureBool_UL = glGetUniformLocation(shaderProgramID, "hasRenderTexture");
    glUniform1f(renderTextureBool_UL, (GLfloat)GL_FALSE);

    GLint textureBool_UL = glGetUniformLocation(shaderProgramID, "hasTexture");
    glUniform1f(textureBool_UL, (GLfloat)GL_FALSE);

    for (int i = 0; i < cMesh::NUM_OF_TEXTURES; i++)
    {
        if (!pCurrentMesh->texture[i].empty())
        {
            glUniform1f(textureBool_UL, (GLfloat)GL_TRUE);
            break;
        }
    }

    for (int i = 0; i < cMesh::NUM_OF_TEXTURES; i++)
    {
        if (pCurrentMesh->texture[i].empty())
        {
            continue;
        }
        std::string texture_UL_name = "texture_0" + std::to_string(i);
        GLint texture_UL = glGetUniformLocation(shaderProgramID, texture_UL_name.c_str());

        glActiveTexture(GL_TEXTURE0 + i);
        GLuint textureNumber = textureManager->getTextureIDFromName(pCurrentMesh->texture[i]);
        glBindTexture(GL_TEXTURE_2D, textureNumber);
        glUniform1i(texture_UL, i);
    }

    GLint textureMixRatio_0_3_UL = glGetUniformLocation(shaderProgramID, "textureMixRatio_0_3");

    glUniform4f(textureMixRatio_0_3_UL,
        pCurrentMesh->textureRatio[0],
        pCurrentMesh->textureRatio[1],
        pCurrentMesh->textureRatio[2],
        pCurrentMesh->textureRatio[3]);

    if (!pCurrentMesh->maskTexture.empty())
    {
        GLint maskBool_UL = glGetUniformLocation(shaderProgramID, "hasMask");
        glUniform1f(maskBool_UL, (GLfloat)GL_TRUE);

        GLint mask_UL = glGetUniformLocation(shaderProgramID, "maskTexture");
        GLint textureUnitNumber = 25;
        GLuint stencilMaskID = textureManager->getTextureIDFromName(pCurrentMesh->maskTexture);
        glActiveTexture(GL_TEXTURE0 + textureUnitNumber);
        glBindTexture(GL_TEXTURE_2D, stencilMaskID);

        GLint bUseDiscardMaskTexture_UL = glGetUniformLocation(shaderProgramID, "maskTexture");
        glUniform1i(bUseDiscardMaskTexture_UL, textureUnitNumber);
    }
    else
    {
		GLint maskBool_UL = glGetUniformLocation(shaderProgramID, "hasMask");
		glUniform1f(maskBool_UL, (GLfloat)GL_FALSE);
	}


   
}

void MeshManager::CalculateMatrices(cMesh* pCurrentMesh, Node* node, const glm::mat4& parentTransformationMatrix, 
    sModelDrawInfo& modelInfo)
{
    std::string nodeName = node->Name;
    glm::mat4 nodeTransform = node->Transformation;  

    std::map<std::string, glm::mat4>::iterator boneIt = 
                        pCurrentMesh->boneTransformations.find(nodeName);

    if (boneIt != pCurrentMesh->boneTransformations.end())
    {
         nodeTransform = boneIt->second;
    }

    glm::mat4 globalTransformation = parentTransformationMatrix * nodeTransform;

    auto boneMapIt = modelInfo.BoneNameToIdMap.find(nodeName);
    if (boneMapIt != modelInfo.BoneNameToIdMap.end())
    {
        BoneInfo& boneInfo = modelInfo.vecBoneInfo[boneMapIt->second];
        boneInfo.FinalTransformation = modelInfo.GlobalInverseTransformation *
            globalTransformation * boneInfo.BoneOffset;
      
    }

    // Calculate all children
    for (int i = 0; i < node->Children.size(); ++i)
    {
        CalculateMatrices(pCurrentMesh, node->Children[i], globalTransformation, modelInfo);
    }

}

