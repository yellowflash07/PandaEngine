#pragma once

#include <vector>
#include "Animation.h"

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
private:
	// The animations in the system
	std::vector<Animation*> m_animations;
	bool m_isPaused;
	bool m_isDone;
	// Update a single animation
	void UpdateAnimation(Animation* animation, float dt);
};

