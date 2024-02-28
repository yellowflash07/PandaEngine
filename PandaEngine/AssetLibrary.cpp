#include "AssetLibrary.h"
#include <windows.h>
#include <iostream>
#include <imgui.h>

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

 /*   for (size_t i = 0; i < m_modelFiles.size(); i++)
    {
        std::wstring file = m_modelFiles[i];
        std::string fileStr(file.begin(), file.end());
        cMesh* mesh = m_meshManager->LoadMesh(fileStr, fileStr,shaderProgramID);
        glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 100.0f);
        Camera* camera = new Camera(cameraPos,
            glm::vec3(0.0f, 0.0f, -1.0f),
            glm::vec3(0.0f, 1.0f, 0.0f), 0.1f, 10000.0f);
        RenderTexture* renderTexture = new RenderTexture(camera, 512, 512, shaderProgramID, { mesh });
        renderTexture->meshManager = m_meshManager;
        m_renderTextures.push_back(renderTexture);
    }*/

}

void AssetLibrary::RenderBox()
{
    ImGui::Begin("Asset Library");
    if (ImGui::BeginTabBar("Assets"))
    {
        if (ImGui::BeginTabItem("Models"))
        {
            for (int i = 0; i < m_modelFiles.size(); i++)
            {
                std::wstring file = m_modelFiles[i];
                std::string fileStr(file.begin(), file.end());
                ImGui::Button(fileStr.c_str(), ImVec2(200, 100));

               // RenderTexture* rt = m_renderTextures[i];
               // rt->Render();
               // ImGui::ImageButton((void*)(intptr_t)rt->GetTextureID(), ImVec2(100, 100));
                if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
                {
                    //std::string fileStr = rt->offScreenMeshList[0]->meshName;
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
