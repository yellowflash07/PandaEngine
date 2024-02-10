#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include "cMesh.h"
#include <functional>

enum class EasingType
{
	Linear,
	SineEaseIn,
	SineEaseOut,
	SineEaseInOut
};

struct KeyFrame
{
	KeyFrame(double time, EasingType type = EasingType::Linear)
		: time(time)
		, easingType(type)
	{ }
	double time;
	EasingType easingType;
	std::function<void()> callback;
};


struct PositionKeyFrame : KeyFrame
{
	PositionKeyFrame(const glm::vec3& position, double time, EasingType type = EasingType::Linear)
		: position(position)
		, KeyFrame(time, type)
	{ }

	glm::vec3 position;
};

struct RotationKeyFrame : KeyFrame
{
	RotationKeyFrame(const glm::vec3& rotation, double time, EasingType type = EasingType::Linear)
		: rotation(rotation)
		, KeyFrame(time, type)
	{ }
	glm::vec3 rotation;
};

struct ScaleKeyFrame : KeyFrame
{
	ScaleKeyFrame(const glm::vec3& scale, double time, EasingType type = EasingType::Linear)
		: scale(scale)
		, KeyFrame(time, type)
	{ }
	glm::vec3 scale;
};

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