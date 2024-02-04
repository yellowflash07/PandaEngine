#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include "cMesh.h"

enum class EasingType
{
	Linear,
	SineEaseIn,
	SineEaseOut,
	SineEaseInOut
};


struct PositionKeyFrame
{
	PositionKeyFrame(const glm::vec3& position, double time, EasingType type = EasingType::Linear)
		: position(position)
		, time(time)
		, easingType(type)
	{ }

	double time;
	glm::vec3 position;
	EasingType easingType;
};

struct RotationKeyFrame
{
	RotationKeyFrame(const glm::vec3& rotation, double time, EasingType type = EasingType::Linear)
		: rotation(rotation)
		, time(time)
		, easingType(type)
	{ }
	double time;
	glm::vec3 rotation;
	EasingType easingType;
};

struct ScaleKeyFrame
{
	ScaleKeyFrame(const glm::vec3& scale, double time, EasingType type = EasingType::Linear)
		: scale(scale)
		, time(time)
		, easingType(type)
	{ }
	double time;
	glm::vec3 scale;
	EasingType easingType;
};

struct Animation
{
	std::string name;
	cMesh* mesh;
	std::vector<PositionKeyFrame> positionKeyFrames;
	std::vector<RotationKeyFrame> rotationKeyFrames;
	std::vector<ScaleKeyFrame> scaleKeyFrames;
	double time;
};