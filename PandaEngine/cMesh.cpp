#include "cMesh.h"
#include <iostream>
#include "../cVAOManager/cVAOManager.h"
#include "../Basic Shader Manager/cShaderManager.h"
// Constructor: Called on creation   c'tor
cMesh::cMesh()
{

	this->bIsWireframe = false;
	this->bDoNotLight = false;

	this->bIsVisible = true;

	this->bUseDebugColours = false;
	this->wholeObjectDebugColourRGBA = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	this->color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	// Set uniqueID
	this->m_UniqueID = cMesh::m_nextUniqueID;
	cMesh::m_nextUniqueID++;
	this->friendlyName = "[Drag and drop model here]";
	this->minExtents_XYZ = glm::vec3(0.0f);
	this->maxExtents_XYZ = glm::vec3(0.0f);
}

unsigned int cMesh::getUniqueID(void)
{
	return this->m_UniqueID;
}

void cMesh::calcExtents(void)
{

	for (sModelDrawInfo modelDrawV : modelDrawInfo)
	{
		sModelDrawInfo modelDraw;

		GetTransformedMeshDrawInfo(modelDraw);

		modelDraw.calcExtents();

		if (modelDraw.minExtents_XYZ.x < this->minExtents_XYZ.x) { this->minExtents_XYZ.x = modelDraw.minExtents_XYZ.x; }
		if (modelDraw.minExtents_XYZ.y < this->minExtents_XYZ.y) { this->minExtents_XYZ.y = modelDraw.minExtents_XYZ.y; }
		if (modelDraw.minExtents_XYZ.z < this->minExtents_XYZ.z) { this->minExtents_XYZ.z = modelDraw.minExtents_XYZ.z; }
						
		if (modelDraw.maxExtents_XYZ.x > this->maxExtents_XYZ.x) { this->maxExtents_XYZ.x = modelDraw.maxExtents_XYZ.x; }
		if (modelDraw.maxExtents_XYZ.y > this->maxExtents_XYZ.y) { this->maxExtents_XYZ.y = modelDraw.maxExtents_XYZ.y; }
		if (modelDraw.maxExtents_XYZ.z > this->maxExtents_XYZ.z) { this->maxExtents_XYZ.z = modelDraw.maxExtents_XYZ.z; }



		//maxExtents_XYZ = modelDraw.maxExtents_XYZ;
		//minExtents_XYZ = modelDraw.minExtents_XYZ;
	}

	

}

void cMesh::AddChild(cMesh* child)
{
	child->isChild = true;
	this->vec_pChildMeshes.push_back(child);
}

bool cMesh::GetTransformedMeshDrawInfo(sModelDrawInfo& transformedInfo)
{

	glm::mat4 matModel = transform.GetTransform();

	sModelDrawInfo drawInfo = this->modelDrawInfo[0];
	transformedInfo = drawInfo;
	for (size_t i = 0; i < drawInfo.numberOfVertices; i++)
	{
		glm::vec3 vert = glm::vec3(drawInfo.pVertices[i].x,
									drawInfo.pVertices[i].y,
									drawInfo.pVertices[i].z);
		vert = (matModel * glm::vec4(vert, 1.0f));

		transformedInfo.pVertices[i].x = vert.x;
		transformedInfo.pVertices[i].y = vert.y;
		transformedInfo.pVertices[i].z = vert.z;
	}

	for (int i = 0; i < drawInfo.numberOfTriangles; i++)
	{
		glm::vec3 v1 = drawInfo.pTriangles[i].v1;
		glm::vec3 v2 = drawInfo.pTriangles[i].v2;
		glm::vec3 v3 = drawInfo.pTriangles[i].v3;
		transformedInfo.pTriangles[i].v1 = (matModel * glm::vec4(v1, 1.0f));
		transformedInfo.pTriangles[i].v2 = (matModel * glm::vec4(v2, 1.0f));
		transformedInfo.pTriangles[i].v3 = (matModel * glm::vec4(v3, 1.0f));
	}

	return true;
}

void cMesh::Render()
{
	ImGui::BeginChild("Mesh", ImVec2(0, 125));
	ImGui::SeparatorText("Mesh Component");
	std::string name = "Mesh: " + friendlyName;
	ImGui::Text(name.c_str());
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Model_DND"))
		{
			const char* payload_n = (const char*)payload->Data;
			LoadMesh(payload_n, payload_n);
			//std::cout << "Accepted: " << payload_n << std::endl;
		}
		ImGui::EndDragDropTarget();
	}
	ImGui::Checkbox("Shadow", &enableShadow);
	ImGui::Checkbox("Wireframe", &bIsWireframe);
	ImGui::Checkbox("Dynamic LOD", &dynamicLOD);
	ImGui::SliderFloat("Transparency", &transperancy, 0.0f, 1.0f);
	for (size_t i = 0; i < NUM_OF_TEXTURES; i++)
	{
		if (texture[i].empty())
		{
			ImGui::Text("Texture Slot: %d EMPTY", i);
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Texture_DND"))
				{
					const char* payload_n = (const char*)payload->Data;
					texture[i] = payload_n;
					textureRatio[i] = 1.0f;
					hasVertexColors = false;
					bUseDebugColours = false;
				}
				ImGui::EndDragDropTarget();
			}
			// break;
		}
		else
		{
			ImGui::Text("Texture Slot: %s", texture[i].c_str());

			if (ImGui::SliderFloat(("Mix Ratio #" + std::to_string(i)).c_str(), &textureRatio[i], 0.0f, 1.0f))
			{
			}
			if (ImGui::Button("Remove Texture"))
			{
				texture[i] = "";
				textureRatio[i] = 0.0f;
			}
		}
	}

	//ImGui::NewLine();
	ImGui::Text("Mask Texture: %s", maskTexture.c_str());
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Texture_DND"))
		{
			const char* payload_n = (const char*)payload->Data;
			maskTexture = payload_n;
		}
		ImGui::EndDragDropTarget();
	}

	if (!maskTexture.empty())
	{
		if (ImGui::Button("Remove Mask"))
		{
			maskTexture = "";
		}
	}
	ImGui::Separator();
	ImGui::EndChild();
}

void cMesh::LoadMesh(std::string fileName, std::string friendlyName)
{
	cVAOManager* pVAOManager = cVAOManager::getInstance();
	//HACK:
	cShaderManager* pShaderManager = cShaderManager::getInstance();
	GLint shaderID = pShaderManager->getIDFromFriendlyName("shader01");
	if (!pVAOManager->LoadModelIntoVAOAI(fileName, this->modelDrawInfo, shaderID))
	{
		std::cout << "Didn't load model" << std::endl;
		return;
	}

	this->meshName = fileName;
	if (modelDrawInfo.empty())
	{
		return;
	}
	this->uniqueName = modelDrawInfo[0].uniqueName;
	this->friendlyName = friendlyName;
	this->bIsWireframe = false;
	this->bDoNotLight = false;
	this->bIsVisible = true;
	this->bUseDebugColours = false;
	this->wholeObjectDebugColourRGBA = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	this->color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	this->isSkyBox = false;
	this->hasVertexColors = false;
	this->isReflective = false;
	this->texture[0] = "";
	this->texture[1] = "";
	this->texture[2] = "";
	this->texture[3] = "";
	this->textureRatio[0] = 1.0f;
	this->textureRatio[1] = 1.0f;
	this->textureRatio[2] = 1.0f;
	this->textureRatio[3] = 1.0f;
}


// static
unsigned int cMesh::m_nextUniqueID = cMesh::FIRST_UNIQUE_ID;


cMesh::cMesh(std::string meshName, std::string friendlyName)
{
	//std::cout << "!!!!!!!!!!!!!!" << std::endl;
	std::cout << meshName << "," << friendlyName << std::endl;
	
	this->LoadMesh(meshName, friendlyName);

	this->minExtents_XYZ = glm::vec3(0.0f);
	this->maxExtents_XYZ = glm::vec3(0.0f);
	this->m_UniqueID = cMesh::m_nextUniqueID;
	cMesh::m_nextUniqueID++;
}

// Destructor: Called on deletion   d'tor
cMesh::~cMesh()
{

}




