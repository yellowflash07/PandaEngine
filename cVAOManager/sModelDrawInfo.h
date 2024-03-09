#pragma once

#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <string>
#include <vector>
#include <map>
#include "../PandaEngine/AnimationFrames.h"
// The vertex structure 
//	that's ON THE GPU (eventually) 
// So dictated from THE SHADER
struct sVertex
{
	sVertex()
	{
		boneIndex[0] = 0;
		boneIndex[1] = 0;
		boneIndex[2] = 0;
		boneIndex[3] = 0;
		boneWeights[0] = 0.0f;
		boneWeights[1] = 0.0f;
		boneWeights[2] = 0.0f;
		boneWeights[3] = 0.0f;
	}

//	float x, y, z;		
//	float r, g, b;
	float x, y, z, w;		// w 'cause math
	float r, g, b, a;
	float nx, ny, nz, nw;	// Won't use nw
	float u, v;				// Texture coordinates

	int boneIndex[4];	// Bone indexes
	float boneWeights[4];	// Bone weights
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
	//glm::mat4 GlobalTransformation = glm::mat4(1.0f);;	// Bone space to world space
};


struct NodeAnimation
{
	NodeAnimation(const std::string& name) : Name(name) { }
	std::string Name;
	std::vector<PositionKeyFrame> PositionKeys;
	std::vector<RotationKeyFrame> RotationKeys;
	std::vector<ScaleKeyFrame> ScalingKeys;
};

struct AnimationInfo
{
	std::string AnimationName;
	float Duration;
	float TicksPerSecond;
	Node* RootNode;
	std::map<std::string, NodeAnimation*> NodeAnimations;
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

	
	glm::vec3 maxExtents_XYZ;	// bounding box maximums
	glm::vec3 minExtents_XYZ;	// bounding box minimums
	glm::vec3 deltaExtents_XYZ;	// bounding box dimensions
	float maxExtent;

	void calcExtents(void);

	Node* RootNode;
	glm::mat4 GlobalInverseTransformation;
	std::vector<BoneInfo> vecBoneInfo;
	std::map<std::string, int> BoneNameToIdMap;
	std::vector<AnimationInfo> Animations;
	unsigned int getUniqueID(void);
private:
	unsigned int m_UniqueID;
	static const unsigned int FIRST_UNIQUE_ID = 1;
	static unsigned int m_nextUniqueID;
};
