#pragma once

#include <vector>
#include "Animation.h"
#include "MeshManager.h"
#include "IEditorUI.h"

class AnimationSystem : public IEditorUI
{
public:
	AnimationSystem();
	~AnimationSystem();

	// Update all animations
	void Update(float dt);
	// Add an animation to the system
	void AddAnimation(Animation* anim);
	// Pause the animation system
	void Pause();
	// Resume the animation system
	void Resume();
	// Set the speed of the animation
	float animationSpeed = 1.0f;	
	// Check if the animation system is paused
	bool IsDone() { return m_isDone; }

	//MeshManager* meshManager;
	float debug = 0.0f;
	void SetAnimation(int index) { currentAnimationIndex = index; }

	void UpdateSkeleton(cMesh* mesh, float dt);

	void UpdateBoneTransforms(sModelDrawInfo* drawInfo, Node& node, float dt);

	//glm::mat4 GetBlendTransform(Node& node, AnimationInfo* start, AnimationInfo* end, float blendFactor, float dt);
	void BlendBoneTransforms(sModelDrawInfo* drawInfo, Node& node, float dt, int animationIndexA, int animationIndexB, float blendWeight);
	
	void SetBlend(int indexA, int indexB, float weight) { blendIndexA = indexA; blendIndexB = indexB; blendWeight = weight; isBlending = true; }
	void Render();

	void SetCurrentAnimation(int index) { currentAnimationIndex = index; }


	void LoadAnimationFromFile(std::string fileName);

	void AttachObjectToBone(std::string boneName, TransformComponent* transform);

	//void SetMesh(cMesh* mesh);
	cMesh* m_mesh;
private:

	sModelDrawInfo* m_drawInfo;
	// The animations in the system
	std::vector<Animation*> m_animations;
	bool m_isPaused;
	bool m_isDone;
	// Update a single animation
	void UpdateAnimation(Animation* animation, float dt);


	glm::vec3 InterpolatePositions(std::vector<PositionKeyFrame> positions,
		float dt);
	glm::vec3 InterpolateScales(std::vector<ScaleKeyFrame> scales,
				float dt);
	glm::quat InterpolateRotations(std::vector<RotationKeyFrame> rotations, float dt);

	int currentAnimationIndex = 0;

	Animation* GetAnimation(AnimationInfo& info, std::string nodeName);

	glm::mat4 InterpolateNodeTransforms(NodeAnimation* nodeAnim, float dt);

	float frameCount = 1.0f;

	bool isBlending = false;

	int blendIndexA = 0;
	int blendIndexB = 0;
	float blendWeight = 0.0f;
};

