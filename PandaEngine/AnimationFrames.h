#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <string>
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