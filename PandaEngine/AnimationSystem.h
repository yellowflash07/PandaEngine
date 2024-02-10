#pragma once

#include <vector>
#include "Animation.h"

class AnimationSystem
{
public:
	AnimationSystem();
	~AnimationSystem();

	void Update(float dt);
	void AddAnimation(Animation* anim);
	void Pause();
	void Resume();
	float animationSpeed = 1.0f;	
private:
	std::vector<Animation*> m_animations;
	bool m_isPaused;
};

