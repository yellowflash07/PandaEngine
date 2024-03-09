#pragma once

#include <vector>
#include "Animation.h"
#include "MeshManager.h"

class AnimationSystem
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

	MeshManager* meshManager;
	float debug = 0.0f;
	void SetAnimation(int index) { currentAnimationIndex = index; }
private:
	// The animations in the system
	std::vector<Animation*> m_animations;
	bool m_isPaused;
	bool m_isDone;
	// Update a single animation
	void UpdateAnimation(Animation* animation, float dt);

	void UpdateBoneTransforms(cMesh* mesh, Node& node,float dt);

	glm::vec3 InterpolatePositions(std::vector<PositionKeyFrame> positions,
		float dt);
	glm::vec3 InterpolateScales(std::vector<ScaleKeyFrame> scales,
				float dt);
	glm::quat InterpolateRotations(std::vector<RotationKeyFrame> rotations, float dt);

	int currentAnimationIndex = 0;

	Animation* GetAnimation(AnimationInfo& info, std::string nodeName);

	glm::mat4 InterpolateNodeTransforms(NodeAnimation* nodeAnim, float dt);
};

