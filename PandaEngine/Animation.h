#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include "cMesh.h"
#include "AnimationFrames.h"

struct Animation
{
	Animation(std::string name, cMesh* mesh)
		: name(name)
		, mesh(mesh)
	{ }

	std::string name;
	cMesh* mesh;
	std::vector<PositionKeyFrame> positionKeyFrames;
	std::vector<RotationKeyFrame> rotationKeyFrames;
	std::vector<ScaleKeyFrame> scaleKeyFrames;
	double time;
	
	bool IsDone()
	{
		if (time >= positionKeyFrames[positionKeyFrames.size() - 1].time
			&& time >= rotationKeyFrames[rotationKeyFrames.size() -1].time
			&& time >= scaleKeyFrames[scaleKeyFrames.size() - 1].time)
		{
			return true;
		}
		return false;
	}
};

struct SkinnedAnimation : Animation
{
	SkinnedAnimation(std::string name, cMesh* mesh)
		: Animation(name, mesh)
	{ }
	std::vector<std::vector<PositionKeyFrame>> bonePositionKeyFrames;
	std::vector<std::vector<RotationKeyFrame>> boneRotationKeyFrames;
	std::vector<std::vector<ScaleKeyFrame>> boneScaleKeyFrames;
};