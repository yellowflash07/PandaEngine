#pragma once

#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <string>
#include <vector>
#include <map>
// The vertex structure 
//	that's ON THE GPU (eventually) 
// So dictated from THE SHADER
struct sVertex
{
//	float x, y, z;		
//	float r, g, b;
	float x, y, z, w;		// w 'cause math
	float r, g, b, a;
	float nx, ny, nz, nw;	// Won't use nw
	float u, v;				// Texture coordinates
	float bx, by, bz, bw;	// Bone indexes
	float tx, ty, tz, tw;	// Bone weights
};

struct sTriangle
{
	glm::vec3 v1;
	glm::vec3 v2;
	glm::vec3 v3;
};

struct Node
{
	Node(const std::string& name) : Name(name) { }
	std::string Name;
	glm::mat4 Transformation;
	std::vector<Node*> Children;
};

struct BoneInfo
{
	std::string boneName;
	unsigned int boneID;		// Index of the bone
	glm::mat4 BoneOffset = glm::mat4(1.0f);;	// How to move from bone space to mesh space
	glm::mat4 FinalTransformation = glm::mat4(1.0f);	// Bone space to world space
	glm::mat4 GlobalTransformation = glm::mat4(1.0f);;	// Bone space to world space
};

struct BoneWeightInfo {
	BoneWeightInfo() {
		m_BoneId[0] = 0;
		m_BoneId[1] = 0;
		m_BoneId[2] = 0;
		m_BoneId[3] = 0;
		m_Weight[0] = 0.f;
		m_Weight[1] = 0.f;
		m_Weight[2] = 0.f;
		m_Weight[3] = 0.f;
	}
	float m_BoneId[4];
	float m_Weight[4];
};

struct sModelDrawInfo
{
	sModelDrawInfo();

	std::string meshName;

	unsigned int VAO_ID;

	unsigned int VertexBufferID;
	unsigned int VertexBuffer_Start_Index;
	unsigned int numberOfVertices;

	unsigned int IndexBufferID;
	unsigned int IndexBuffer_Start_Index;
	unsigned int numberOfIndices;
	unsigned int numberOfTriangles;

	// The "local" (i.e. "CPU side" temporary array)
	sVertex* pVertices;	//  = 0;
	// The index buffer (CPU side)
	unsigned int* pIndices;	// Triangles

	sTriangle* pTriangles;

	std::vector<BoneInfo> vecBoneInfo;
	std::vector<BoneWeightInfo> vecBoneWeights;
	std::map<std::string, int> BoneNameToIdMap;
	glm::vec3 maxExtents_XYZ;	// bounding box maximums
	glm::vec3 minExtents_XYZ;	// bounding box minimums
	glm::vec3 deltaExtents_XYZ;	// bounding box dimensions
	float maxExtent;

	void calcExtents(void);
	Node* RootNode;
	std::map<std::string, int> NodeNameToIdMap;
	std::vector<glm::mat4> NodeHeirarchyTransformations;
	glm::mat4 GlobalInverseTransformation;
	// 
	unsigned int getUniqueID(void);
private:
	unsigned int m_UniqueID;
	static const unsigned int FIRST_UNIQUE_ID = 1;
	static unsigned int m_nextUniqueID;
};
