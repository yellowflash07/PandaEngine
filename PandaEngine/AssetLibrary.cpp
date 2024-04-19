#include "AssetLibrary.h"
#include <windows.h>
#include <iostream>
#include <imgui.h>

#define THREADED
extern bool IMGUI_ENABLE;
AssetLibrary::AssetLibrary()
{
    m_modelFiles = GetFiles("../Assets/Models");
    m_texFiles = GetFiles("../Assets/Textures");
}

AssetLibrary::~AssetLibrary()
{
    
}

void AssetLibrary::Init()
{
    for (int i = 0; i < m_texFiles.size(); i++)
    {
        std::wstring file = m_texFiles[i];
        std::string fileStr(file.begin(), file.end());
        m_texManager->Create2DTextureFromBMPFile(fileStr, true);
        GLuint texID = m_texManager->getTextureIDFromName(fileStr);
        m_texIDs.push_back(texID);
    }

    if (!IMGUI_ENABLE) return;

    for (size_t i = 0; i < m_modelFiles.size(); i++)
    {
        std::wstring file = m_modelFiles[i];
        std::string fileStr(file.begin(), file.end());
      
        glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 100.0f);
        Camera* camera = new Camera(cameraPos,
            glm::vec3(0.0f, 0.0f, -1.0f),
            glm::vec3(0.0f, -1.0f, 0.0f), 0.1f, 10000.0f);
#ifndef THREADED

        cMesh* mesh = m_meshManager->LoadMesh(fileStr, fileStr, this->shaderProgramID);
        mesh->bUseDebugColours = true;
        mesh->wholeObjectDebugColourRGBA = glm::vec4(0.7f, 0.7f, 0.7f, 1.0f);
        mesh->calcExtents();

        glm::vec3 extents = mesh->maxExtents_XYZ - mesh->minExtents_XYZ;

        float maxExtent = extents.x;

        camera->cameraEye = mesh->GetCenter() + glm::vec3(0.0f, 0.0f, maxExtent * 2.0f);

        RenderTexture* rt = new RenderTexture(camera, 800, 600, shaderProgramID, { mesh });
        m_renderTextures.push_back(rt);
        rt->meshManager = this->m_meshManager;

#endif // !THREADED


#ifdef THREADED
        GLuint spid = this->shaderProgramID;

        std::vector<RenderTexture*>* m_rT = &this->m_renderTextures;
        MeshManager* mM = this->m_meshManager;

        m_meshManager->LoadMeshAsync(fileStr, fileStr, shaderProgramID,
        [ camera, spid, m_rT, mM](cMesh* mesh)
        {
            mesh->bDoNotLight= true;
            mesh->bUseDebugColours = true;
            mesh->wholeObjectDebugColourRGBA = glm::vec4(0.7f, 0.7f, 0.7f, 1.0f);
            mesh->calcExtents();

            glm::vec3 extents = mesh->maxExtents_XYZ - mesh->minExtents_XYZ;

            float maxExtent = extents.x;

            camera->cameraEye = mesh->GetCenter() + glm::vec3(0.0f, 0.0f, maxExtent * 2.0f);

            RenderTexture* rt = new RenderTexture(camera, 800, 600, spid, { mesh });
            m_rT->push_back(rt);
            rt->meshManager = mM;
        });
#endif // THREADED
    }

}

void AssetLibrary::RenderBox()
{
    ImGui::Begin("Asset Library");
    if (ImGui::BeginTabBar("Assets"))
    {
        if (ImGui::BeginTabItem("Models"))
        {
            for (int i = 0; i < m_renderTextures.size(); i++)
            {
              //  std::wstring file = m_modelFiles[i];
              //  std::string fileStr(file.begin(), file.end());
               // ImGui::Button(fileStr.c_str(), ImVec2(200, 100));

                RenderTexture* rt = m_renderTextures[i];
                rt->Render();
                ImGui::ImageButton((void*)(intptr_t)rt->GetTextureID(), ImVec2(100, 100));
                if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
                {
                    std::string fileStr = rt->offScreenMeshList[0]->meshName;
                    // Set payload to carry the index of our item (could be anything)
                    ImGui::SetDragDropPayload("Model_DND", fileStr.c_str(), fileStr.length() * sizeof(char*));
                    ImGui::EndDragDropSource();
                }
                ImGui::SameLine();

                if (i % 3 == 0 && i > 0)
                {
                    ImGui::NewLine();
                }
            }

            ImGui::EndTabItem();
        }


        if (ImGui::BeginTabItem("Textures"))
        {
            for (int i = 0; i < m_texIDs.size(); i++)
            {

                GLuint texID = m_texIDs[i];
                ImGui::ImageButton((void*)(intptr_t)texID, ImVec2(100, 100));
                ImGui::SameLine();
                if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
                {
                    std::wstring file = m_texFiles[i];
                    std::string fileStr(file.begin(), file.end());
                    ImGui::SetDragDropPayload("Texture_DND", fileStr.c_str(), fileStr.length() * sizeof(char*));
                    ImGui::EndDragDropSource();
                }
                if (i % 3 == 0 && i > 0)
                {
                    ImGui::NewLine();
                }
            }
            ImGui::EndTabItem();

        }
    }
    
    ImGui::EndTabBar();
	ImGui::End();

}

std::vector<std::wstring> AssetLibrary::GetFiles(std::string folder)
{
    std::vector<std::wstring> names;
    std::wstring search_path = std::wstring(folder.begin(), folder.end()) + L"\\*";
    WIN32_FIND_DATA fd;
    HANDLE hFind = ::FindFirstFile(search_path.c_str(), &fd);
    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            // read all (real) files in current folder
            // , delete '!' read other 2 default folder . and ..
            if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                names.push_back(fd.cFileName);
            }
        } while (::FindNextFile(hFind, &fd));
        ::FindClose(hFind);
    }
    return names;
}
