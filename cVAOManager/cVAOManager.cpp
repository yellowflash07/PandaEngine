#include "cVAOManager.h"


#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/quaternion.hpp>

#include <vector>

#include <sstream>
#include <fstream>
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

#include <iostream>
#include <chrono>
#include "../PandaEngine/Debug.h"
static cVAOManager* instance = nullptr;

CRITICAL_SECTION g_cs;

struct LoadInfo
{
    LoadInfo(sModelDrawInfo& drawInfo) : drawInfo(drawInfo) {}

    cVAOManager* vaoManager;
    std::string fileName;
    sModelDrawInfo& drawInfo;
    std::vector<sModelDrawInfo> resultInfo;
    GLuint shaderProgramID;
    bool result = false;
    std::function<void()> callback;
    OnModelLoadCallBack onModelLoadCallBack;
};

DWORD WINAPI LoadTheFileAsync(LPVOID lpParameter)
{
    LoadInfo* loadInfo = (LoadInfo*)lpParameter;
    cVAOManager* vaoManager = loadInfo->vaoManager;

    loadInfo->result = vaoManager->m_LoadTheFile(loadInfo->fileName, loadInfo->resultInfo);

   // std::cout << "Thread completed" << std::endl;
    return 0;
}


void cVAOManager::setBasePath(std::string basePathWithoutSlash)
{
    this->m_basePathWithoutSlash = basePathWithoutSlash;
    InitializeCriticalSection(&g_cs);
    return;
}


cVAOManager::cVAOManager()
{
    instance = this;
}

cVAOManager* cVAOManager::getInstance()
{
    return instance;
}

bool cVAOManager::LoadModelIntoVAOAI(
    std::string fileName,
    std::vector<sModelDrawInfo>& drawInfos,
    unsigned int shaderProgramID,
    bool bIsDynamicBuffer /*=false*/)

{
    // Load the model from file
    if (!m_LoadTheFile(fileName, drawInfos))
    {
		return false;
	}

    // Load the model into the GPU

    for (size_t i = 0; i < drawInfos.size(); i++)
    {
        sModelDrawInfo* drawInfo = &drawInfos[i];
		LoadVertexToGPU(drawInfo, shaderProgramID);
        std::string modelName = GenerateUniqueModelNameFromFile(fileName);
        m_map_ModelName_to_VAOID[modelName] = drawInfos[i];
        drawInfos[i].uniqueName = modelName;
	}

   // LoadVertexToGPU(drawInfo, shaderProgramID);

    // Store the draw information into the map
   

    return true;
}

bool cVAOManager::LoadModelIntoVAOAsync(std::string fileName, sModelDrawInfo& drawInfo, 
    unsigned int shaderProgramID, OnModelLoadCallBack callback, bool bIsDynamicBuffer)
{
    LoadInfo* loadInfo = new LoadInfo(drawInfo);
    loadInfo->vaoManager = this;
    loadInfo->fileName = fileName;
    loadInfo->shaderProgramID = shaderProgramID;
    loadInfo->onModelLoadCallBack = callback;

    HANDLE handle = CreateThread(NULL, 0, LoadTheFileAsync, loadInfo, 0, NULL);

    loadQueue.push_back(loadInfo);
    return true;
}

void cVAOManager::CheckQueue()
{
    for (size_t i = 0; i < loadQueue.size(); i++)
    {
        LoadInfo* loadInfo = loadQueue[i];

        if (loadInfo->result)
        {
            std::vector<sModelDrawInfo>& drawInfos = loadInfo->resultInfo;
            for (size_t i = 0; i < drawInfos.size(); i++)
            {
                sModelDrawInfo* drawInfo = &drawInfos[i];
                LoadVertexToGPU(drawInfo, loadInfo->shaderProgramID);
                std::string modelName = GenerateUniqueModelNameFromFile(loadInfo->fileName);
                m_map_ModelName_to_VAOID[modelName] = drawInfos[i];
                drawInfo->uniqueName = modelName;
            }

			loadQueue.erase(loadQueue.begin() + i);

            loadInfo->onModelLoadCallBack(loadInfo->resultInfo);
           // loadInfo->callback();
		}

    }
}

bool cVAOManager::FindDrawInfoByModelName(
		std::string filename,
		sModelDrawInfo &drawInfo) 
{
	std::map< std::string /*model name*/,
			sModelDrawInfo /* info needed to draw*/ >::iterator 
		itDrawInfo = this->m_map_ModelName_to_VAOID.find( filename );

	// Find it? 
	if ( itDrawInfo == this->m_map_ModelName_to_VAOID.end() )
	{
		// Nope
		return false;
	}

	// Else we found the thing to draw
	// ...so 'return' that information
	drawInfo = itDrawInfo->second;
	return true;
}



bool cVAOManager::m_LoadTheFile(std::string fileName, std::vector<sModelDrawInfo>& drawInfos)
{
   // std::cout << "Loading: " << fileName << std::endl;
    Assimp::Importer importer;
    std::string filePath = this->m_basePathWithoutSlash + "/" + fileName;
    const aiScene* scene = importer.ReadFile(filePath, aiProcess_ValidateDataStructure | aiProcess_GenNormals | aiProcess_Triangulate
                                                       | aiProcess_PopulateArmatureData | aiProcess_CalcTangentSpace | aiProcess_FlipUVs);

    std::string errorString = importer.GetErrorString();
    if (!errorString.empty())
    {
		std::cout << "Error: " << importer.GetErrorString() << std::endl;
	}

    if (!scene)
    {
        // Error loading the model
        // Handle the error as needed
        return false;
    }

   // const aiMesh* mesh = scene->mMeshes[0]; // Assuming there's only one mesh in the scene

  
    // Allocate memory for vertices and indices
   



    for (int i = 0; i < scene->mNumMeshes; i++)
    {
        sModelDrawInfo drawInfo;
        EnterCriticalSection(&g_cs);
        drawInfo.meshName = fileName; // You can change this as needed
        LeaveCriticalSection(&g_cs);
		LoadMeshes(scene->mMeshes[i], scene, drawInfo);
        drawInfos.push_back(drawInfo);
	}



   // LoadMeshes(scene->mMeshes[0], scene, drawInfo);
 

 /*   std::cout << "Loaded: " << fileName 
                << "Vertices: " << drawInfo.numberOfVertices << std::endl;*/

    return true;
}

void cVAOManager::LoadVertexToGPU(sModelDrawInfo* drawInfo, GLuint shaderProgramID)
{  
    // Create a VAO (Vertex Array Object), which will 
//	keep track of all the 'state' needed to draw 
//	from this buffer...

// Ask OpenGL for a new buffer ID...
    glGenVertexArrays(1, &(drawInfo->VAO_ID));
   

    // "Bind" this buffer:
    // - aka "make this the 'current' VAO buffer
    glBindVertexArray(drawInfo->VAO_ID);

    // Now ANY state that is related to vertex or index buffer
    //	and vertex attribute layout, is stored in the 'state' 
    //	of the VAO... 

    glGenBuffers(1, &(drawInfo->VertexBufferID));

    glBindBuffer(GL_ARRAY_BUFFER, drawInfo->VertexBufferID);

    glBufferData(GL_ARRAY_BUFFER,
        sizeof(sVertex) * drawInfo->numberOfVertices,
        (GLvoid*)drawInfo->pVertices,
        (GL_STATIC_DRAW));


    // Copy the index buffer into the video card, too
    // Create an index buffer.
    glGenBuffers(1, &(drawInfo->IndexBufferID));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, drawInfo->IndexBufferID);

    glBufferData(GL_ELEMENT_ARRAY_BUFFER,			// Type: Index element array
        sizeof(unsigned int) * drawInfo->numberOfIndices,
        (GLvoid*)drawInfo->pIndices,
        GL_STATIC_DRAW);

    // Set the vertex attributes.

    GLint vcol_location = glGetAttribLocation(shaderProgramID, "vCol");	// program;
    GLint vpos_location = glGetAttribLocation(shaderProgramID, "vPos");	// program
    GLint vNormal_location = glGetAttribLocation(shaderProgramID, "vNormal");	// program;
    GLint vTexureCoord_location = glGetAttribLocation(shaderProgramID, "vTexCoord");	// program;

    // Set the vertex attributes for this shader
    glEnableVertexAttribArray(vpos_location);	    // vPos
    glVertexAttribPointer(vpos_location, 4,		// vPos
        GL_FLOAT, GL_FALSE,
        sizeof(sVertex),
        (void*)offsetof(sVertex, x));

    glEnableVertexAttribArray(vcol_location);	    // vCol
    glVertexAttribPointer(vcol_location, 4,		// vCol
        GL_FLOAT, GL_FALSE,
        sizeof(sVertex),
        (void*)offsetof(sVertex, r));

    glEnableVertexAttribArray(vNormal_location);	// vNormal
    glVertexAttribPointer(vNormal_location, 4,		// vNormal
        GL_FLOAT, GL_FALSE,
        sizeof(sVertex),
        (void*)offsetof(sVertex, nx));

    glEnableVertexAttribArray(vTexureCoord_location);	// vTexCoord
    glVertexAttribPointer(vTexureCoord_location, 2,		// vTexCoord
        GL_FLOAT, GL_FALSE,
        sizeof(sVertex),
        (void*)offsetof(sVertex, u));

    glEnableVertexAttribArray(5);	// vBoneID
    glVertexAttribPointer(5, 4,		// vBoneID
        GL_INT, GL_FALSE,
        sizeof(sVertex),
        (void*)offsetof(sVertex, boneIndex));

    glEnableVertexAttribArray(6);		// vBoneWeight
    //glVertexAttribPointer(layout, numComponents, type, GL_FALSE, stride, offset);

    glVertexAttribPointer(6, 4,		// vBoneWeight
        GL_FLOAT, GL_FALSE,
        sizeof(sVertex),
        (void*)offsetof(sVertex, boneWeights));

    glEnableVertexAttribArray(7);		// vTangents

    glVertexAttribPointer(7, 4,		// vTangents
        GL_FLOAT, GL_FALSE,
        sizeof(sVertex),
        (void*)offsetof(sVertex, tx));

    glEnableVertexAttribArray(8);		// vBiTangents

    glVertexAttribPointer(8, 4,		// vBiTangents
        		GL_FLOAT, GL_FALSE,
        		sizeof(sVertex),
        		(void*)offsetof(sVertex, bx));

    //glVertexAttribPointer(6, 4, sizeof(sVertex), GL_FALSE, stride, offset);
    //glVertexAttribPointer(6, 4,		// vBoneWeight
    //    				GL_FLOAT, GL_FALSE,
    //    				sizeof(sVertex),
    //    				(void*)offsetof(sVertex, tx));

    // Now that all the parts are set up, set the VAO to zero
    glBindVertexArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glDisableVertexAttribArray(vpos_location);
    glDisableVertexAttribArray(vcol_location);
    glDisableVertexAttribArray(vNormal_location);
    glDisableVertexAttribArray(vTexureCoord_location);
    glDisableVertexAttribArray(5);
    glDisableVertexAttribArray(6);
    glDisableVertexAttribArray(7);
    glDisableVertexAttribArray(8);
   
}

bool cVAOManager::UpdateVAOBuffers(std::string fileName,
                      sModelDrawInfo& updatedDrawInfo,
                      unsigned int shaderProgramID)
{
    // This exists? 
    sModelDrawInfo updatedDrawInfo_TEMP;
    if ( ! this->FindDrawInfoByModelName(fileName,  updatedDrawInfo_TEMP) )
    {
        // Didn't find this buffer
        return false;
    }


    glBindBuffer(GL_ARRAY_BUFFER, updatedDrawInfo.VertexBufferID);

// Original call to create and copy the initial data:
//     
//    glBufferData(GL_ARRAY_BUFFER,
//                 sizeof(sVertex) * updatedDrawInfo.numberOfVertices,	
//                 (GLvoid*)updatedDrawInfo.pVertices,					
//                 GL_DYNAMIC_DRAW);

    glBufferSubData(GL_ARRAY_BUFFER, 
                    0,  // Offset
                    sizeof(sVertex) * updatedDrawInfo.numberOfVertices,	
                    (GLvoid*)updatedDrawInfo.pVertices);

    glBindBuffer(GL_ARRAY_BUFFER, 0);


    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, updatedDrawInfo.IndexBufferID);

    //glBufferSubData(GL_ELEMENT_ARRAY_BUFFER,			
    //                0,  
    //                sizeof(unsigned int) * updatedDrawInfo.numberOfIndices,
    //                (GLvoid*)updatedDrawInfo.pIndices);

    //glBindBuffer(GL_ARRAY_BUFFER, 0);


    return true;
}

void cVAOManager::AssimpToGLM(const aiMatrix4x4& a, glm::mat4& g)
{
    g[0][0] = a.a1; g[1][0] = a.a2; g[2][0] = a.a3; g[3][0] = a.a4;
    g[0][1] = a.b1; g[1][1] = a.b2; g[2][1] = a.b3; g[3][1] = a.b4;
    g[0][2] = a.c1; g[1][2] = a.c2; g[2][2] = a.c3; g[3][2] = a.c4;
    g[0][3] = a.d1; g[1][3] = a.d2; g[2][3] = a.d3; g[3][3] = a.d4;
}

Node* cVAOManager::GenerateBoneHierarchy(const aiNode* node)
{
    Node* newNode = new Node(node->mName.C_Str());
    aiNode* parentNode = node->mParent;

    if (parentNode)
    {
		Node* parent = new Node(parentNode->mName.C_Str());
        AssimpToGLM(parentNode->mTransformation, parent->Transformation);
		newNode->Parent = parent;
	}
    else
    {
        newNode->Parent = nullptr;
    }

    AssimpToGLM(node->mTransformation, newNode->Transformation);   
    for (int i = 0; i < node->mNumChildren; i++)
    {
        newNode->Children.emplace_back(GenerateBoneHierarchy(node->mChildren[i]));
    }

    return newNode;
}

void cVAOManager::LoadMeshes(aiMesh* mesh, const aiScene* scene, sModelDrawInfo& drawInfo)
{
    drawInfo.numberOfVertices = mesh->mNumVertices;
    drawInfo.pVertices = new sVertex[drawInfo.numberOfVertices];
#pragma region AnimationLoading
    if (scene->HasAnimations())
    {
        for (unsigned int i = 0; i < scene->mNumAnimations; i++)
        {
            aiAnimation* pAnimation = scene->mAnimations[i];
            AnimationInfo animInfo;
            animInfo.AnimationName = pAnimation->mName.C_Str();
            animInfo.Duration = (float)pAnimation->mDuration;
            animInfo.TicksPerSecond = (float)pAnimation->mTicksPerSecond;
         //   animInfo.RootNode = GenerateBoneHierarchy(scene->mRootNode);
            for (unsigned int j = 0; j < pAnimation->mNumChannels; j++)
            {
                aiNodeAnim* pNodeAnim = pAnimation->mChannels[j];
                NodeAnimation* nodeAnim = new NodeAnimation(pNodeAnim->mNodeName.C_Str());
                for (unsigned int k = 0; k < pNodeAnim->mNumPositionKeys; k++)
                {
                    glm::vec3 pos = glm::vec3(pNodeAnim->mPositionKeys[k].mValue.x,
                        pNodeAnim->mPositionKeys[k].mValue.y,
                        pNodeAnim->mPositionKeys[k].mValue.z);
                    PositionKeyFrame keyFrame(pos, (float)pNodeAnim->mPositionKeys[k].mTime);
                    EnterCriticalSection(&g_cs);
                    nodeAnim->PositionKeys.push_back(keyFrame);
                    LeaveCriticalSection(&g_cs);
                }
                for (unsigned int k = 0; k < pNodeAnim->mNumRotationKeys; k++)
                {
                    glm::quat rotQuat = glm::quat(  (float)pNodeAnim->mRotationKeys[k].mValue.w,
                                                    (float)pNodeAnim->mRotationKeys[k].mValue.x,
                                                    (float)pNodeAnim->mRotationKeys[k].mValue.y,
                                                    (float)pNodeAnim->mRotationKeys[k].mValue.z);

                    RotationKeyFrame keyFrame(rotQuat, (float)pNodeAnim->mRotationKeys[k].mTime);
                    
                    EnterCriticalSection(&g_cs);
                    nodeAnim->RotationKeys.push_back(keyFrame);
                    LeaveCriticalSection(&g_cs);
                }
                for (unsigned int k = 0; k < pNodeAnim->mNumScalingKeys; k++)
                {
                    glm::vec3 scale = glm::vec3(pNodeAnim->mScalingKeys[k].mValue.x,
                        pNodeAnim->mScalingKeys[k].mValue.y,
                        pNodeAnim->mScalingKeys[k].mValue.z);
                    ScaleKeyFrame keyFrame(scale, (float)pNodeAnim->mScalingKeys[k].mTime);
                    EnterCriticalSection(&g_cs);
                    nodeAnim->ScalingKeys.push_back(keyFrame);
                    LeaveCriticalSection(&g_cs);
                }
                EnterCriticalSection(&g_cs);
                animInfo.NodeAnimations.insert(std::pair<std::string, NodeAnimation*>(nodeAnim->Name, nodeAnim));
            	LeaveCriticalSection(&g_cs);
            }
            EnterCriticalSection(&g_cs);
            drawInfo.Animations.push_back(animInfo);
            LeaveCriticalSection(&g_cs);
        }
    }
#pragma endregion

    bool calculatedTangents = false;
    for (unsigned int i = 0; i < drawInfo.numberOfVertices; ++i)
    {
        const aiVector3D& vertex = mesh->mVertices[i];
        const aiVector3D& normal = mesh->mNormals[i];
        const aiColor4D& color = mesh->mColors[0][i]; // Assuming there's only one color


        drawInfo.pVertices[i].x = vertex.x;
        drawInfo.pVertices[i].y = vertex.y;
        drawInfo.pVertices[i].z = vertex.z;

        if (mesh->HasVertexColors(0))
        {
            drawInfo.pVertices[i].r = color.r;
            drawInfo.pVertices[i].g = color.g;
            drawInfo.pVertices[i].b = color.b;
            drawInfo.pVertices[i].a = color.a;
        }

        if (mesh->HasNormals())
        {
            drawInfo.pVertices[i].nx = normal.x;
            drawInfo.pVertices[i].ny = normal.y;
            drawInfo.pVertices[i].nz = normal.z;
        }

        if (mesh->HasTextureCoords(0))
        {
            drawInfo.pVertices[i].u = mesh->mTextureCoords[0][i].x;
            drawInfo.pVertices[i].v = mesh->mTextureCoords[0][i].y;
        }

        if (mesh->HasTangentsAndBitangents()) 
        {
            //lucky us, we have tangents and bitangents. no need to calculate them
            drawInfo.pVertices[i].tx = mesh->mTangents[i].x;
			drawInfo.pVertices[i].ty = mesh->mTangents[i].y;
			drawInfo.pVertices[i].tz = mesh->mTangents[i].z;

			drawInfo.pVertices[i].bx = mesh->mBitangents[i].x;
			drawInfo.pVertices[i].by = mesh->mBitangents[i].y;
			drawInfo.pVertices[i].bz = mesh->mBitangents[i].z;
            calculatedTangents = true;
        }

    }
#pragma region BONE LOADING


    if (mesh->HasBones())
    {
        aiNode* root = scene->mRootNode;
        drawInfo.RootNode = GenerateBoneHierarchy(root);
        drawInfo.GlobalInverseTransformation = glm::inverse(drawInfo.RootNode->Transformation);
        
        unsigned int numBones = mesh->mNumBones;
        for (unsigned int boneIdx = 0; boneIdx < numBones; ++boneIdx)
        {
            aiBone* bone = mesh->mBones[boneIdx];

            std::string name(bone->mName.C_Str(), bone->mName.length); //	'\0'
            drawInfo.BoneNameToIdMap.insert(std::pair<std::string, int>(name, drawInfo.vecBoneInfo.size()));

            // Store the offset matrices
            BoneInfo info;
            info.boneName = name;
            AssimpToGLM(bone->mOffsetMatrix, info.BoneOffset);
            drawInfo.vecBoneInfo.emplace_back(info);

            for (int weightIdx = 0; weightIdx < bone->mNumWeights; ++weightIdx)
            {
                aiVertexWeight& vertexWeight = bone->mWeights[weightIdx];

                sVertex& vertex = drawInfo.pVertices[vertexWeight.mVertexId];

                for (int infoIdx = 0; infoIdx < 4; ++infoIdx)
                {
                    if (vertex.boneWeights[infoIdx] <= 0.f)
                    {
                        vertex.boneIndex[infoIdx] = boneIdx;
                        vertex.boneWeights[infoIdx] = vertexWeight.mWeight;
                        break;
                    }
                }
            }
        }
    }


#pragma endregion

    drawInfo.numberOfIndices = mesh->mNumFaces * 3; // Triangles assumed
    drawInfo.pIndices = new unsigned int[drawInfo.numberOfIndices];
    drawInfo.numberOfTriangles = mesh->mNumFaces;
    drawInfo.pTriangles = new sTriangle[drawInfo.numberOfTriangles];

    for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
    {
        const aiFace& face = mesh->mFaces[i];
        drawInfo.pIndices[i * 3] = face.mIndices[0];
        drawInfo.pIndices[i * 3 + 1] = face.mIndices[1];
        drawInfo.pIndices[i * 3 + 2] = face.mIndices[2];

        drawInfo.pTriangles[i].v1.x = mesh->mVertices[face.mIndices[0]].x;
        drawInfo.pTriangles[i].v1.y = mesh->mVertices[face.mIndices[0]].y;
        drawInfo.pTriangles[i].v1.z = mesh->mVertices[face.mIndices[0]].z;

        drawInfo.pTriangles[i].v2.x = mesh->mVertices[face.mIndices[1]].x;
        drawInfo.pTriangles[i].v2.y = mesh->mVertices[face.mIndices[1]].y;
        drawInfo.pTriangles[i].v2.z = mesh->mVertices[face.mIndices[1]].z;

        drawInfo.pTriangles[i].v3.x = mesh->mVertices[face.mIndices[2]].x;
        drawInfo.pTriangles[i].v3.y = mesh->mVertices[face.mIndices[2]].y;
        drawInfo.pTriangles[i].v3.z = mesh->mVertices[face.mIndices[2]].z;

    }
}

std::string cVAOManager::GenerateUniqueModelNameFromFile(std::string fileName)
{
    std::string modelName = fileName;
	std::stringstream ss;
    EnterCriticalSection(&g_cs);
	ss << fileName << "_" << this->m_map_ModelName_to_VAOID.size();
    LeaveCriticalSection(&g_cs);
	modelName = ss.str();
	return modelName;
}

void cVAOManager::PrintMatrix(glm::mat4 theMatrix)
{
    for (int i = 0; i < 4; ++i)
    {
        printf("|");
        for (int j = 0; j < 4; ++j)
        {
            printf("%.2f ", theMatrix[i][j]);
        }
        printf("|\n");
    }
}

void cVAOManager::PrintMatrix(aiMatrix4x4 theMatrix)
{
    for (int i = 0; i < 4; ++i)
    {
		printf("|");
        for (int j = 0; j < 4; ++j)
        {
			printf("%.2f ", theMatrix[i][j]);
		}
		printf("|\n");
	}
}



