#include "AnimationSystem.h"
#include <glm/gtx/easing.hpp>
#include <iostream>

AnimationSystem::AnimationSystem()
{
}

AnimationSystem::~AnimationSystem()
{
}

void AnimationSystem::Update(float dt)
{
	// Update all animations
	for (Animation* anim : m_animations)
	{
		// Skip if paused
		if (m_isPaused)
		{
			continue;
		}
		//std::cout << dt << std::endl;
		UpdateAnimation(anim, dt);
		UpdateBoneTransforms(anim->mesh, *anim->mesh->modelDrawInfo.RootNode, dt);
	}

}

void AnimationSystem::AddAnimation(Animation* anim)
{
	// Add an animation to the system
	m_animations.push_back(anim);
}


void AnimationSystem::Pause()
{
	// Pause the animation system
	m_isPaused = true;
}

void AnimationSystem::Resume()
{
	// Resume the animation system
	m_isPaused = false;
}



void AnimationSystem::UpdateAnimation(Animation* anim, float dt)
{
	// Update the animation
	anim->time = dt;
	double time = anim->time * animationSpeed;
	cMesh* affectedMesh = anim->mesh;
//#pragma region POSITION
//
//
//	// Position
//	if (anim->positionKeyFrames.size() == 1)
//	{
//		affectedMesh->drawPosition = anim->positionKeyFrames[0].position;
//	}
//	else
//	{
//		// Find our position keyframes (Start and End)
//		int KeyFrameEndIndex = 0;
//		for (; KeyFrameEndIndex < anim->positionKeyFrames.size(); KeyFrameEndIndex++)
//		{
//			if (anim->positionKeyFrames[KeyFrameEndIndex].time > time)
//			{
//				break;
//			}
//		}
//
//		if (KeyFrameEndIndex >= anim->positionKeyFrames.size())
//		{
//			// we are at or past the last key frame use the last keyframe only
//			affectedMesh->drawPosition = anim->positionKeyFrames[KeyFrameEndIndex - 1].position;
//			//return;
//		}
//		else
//		{
//			int KeyFrameStartIndex = KeyFrameEndIndex - 1;
//
//			PositionKeyFrame startKeyFrame = anim->positionKeyFrames[KeyFrameStartIndex];
//			PositionKeyFrame endKeyFrame = anim->positionKeyFrames[KeyFrameEndIndex];
//
//			float percent = (time - startKeyFrame.time) / (endKeyFrame.time - startKeyFrame.time);
//			float result = 0.f;
//
//			// Check if we are at the end of frame
//			if (percent > 0.99f)
//			{
//				if (startKeyFrame.callback)
//				{
//					startKeyFrame.callback();
//				}
//			}
//
//			switch (endKeyFrame.easingType)
//			{
//				case EasingType::Linear:
//					result = percent;
//					break;
//
//				case EasingType::SineEaseIn:
//					result = glm::sineEaseIn(percent);
//					break;
//
//				case EasingType::SineEaseOut:
//					result = glm::sineEaseOut(percent);
//					break;
//
//				case EasingType::SineEaseInOut:
//					result = glm::sineEaseInOut(percent);
//					break;
//			}
//
//			glm::vec3 delta = endKeyFrame.position - startKeyFrame.position;
//			affectedMesh->drawPosition = startKeyFrame.position + delta * result;
//		}
//		
//	}
//#pragma endregion
//
//#pragma region ROTATION
//	// Rotation
//	if (anim->rotationKeyFrames.size() == 1)
//	{
//		affectedMesh->setRotationFromEuler(anim->rotationKeyFrames[0].rotation);
//	}
//	else
//	{
//		// Find our position keyframes (Start and End)
//		int KeyFrameEndIndex = 0;
//		for (; KeyFrameEndIndex < anim->rotationKeyFrames.size(); KeyFrameEndIndex++)
//		{
//			if (anim->rotationKeyFrames[KeyFrameEndIndex].time > time)
//			{
//				break;
//			}
//		}
//
//		if (KeyFrameEndIndex >= anim->rotationKeyFrames.size())
//		{
//			// we are at or past the last key frame use the last keyframe only
//			affectedMesh->setRotationFromEuler(anim->rotationKeyFrames[KeyFrameEndIndex - 1].rotation);
//			//return;
//		}
//		else
//		{
//			int KeyFrameStartIndex = KeyFrameEndIndex - 1;
//
//			RotationKeyFrame startKeyFrame = anim->rotationKeyFrames[KeyFrameStartIndex];
//			RotationKeyFrame endKeyFrame = anim->rotationKeyFrames[KeyFrameEndIndex];
//
//			float percent = (time - startKeyFrame.time) / (endKeyFrame.time - startKeyFrame.time);
//			float result = 0.f;
//
//			// Check if we are at the end of frame
//			if (percent > 0.99f)
//			{
//				if (startKeyFrame.callback)
//				{
//					startKeyFrame.callback();
//				}
//			}
//
//			switch (endKeyFrame.easingType)
//			{
//				case EasingType::Linear:
//					result = percent;
//					anim->mesh->wholeObjectDebugColourRGBA = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
//					break;
//				case EasingType::SineEaseIn:
//					result = glm::sineEaseIn(percent);
//					anim->mesh->wholeObjectDebugColourRGBA = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
//					break;
//				case EasingType::SineEaseOut:
//					result = glm::sineEaseOut(percent);
//					//yellow
//					anim->mesh->wholeObjectDebugColourRGBA = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
//					break;
//				case EasingType::SineEaseInOut:
//					result = glm::sineEaseInOut(percent);
//					//green
//					anim->mesh->wholeObjectDebugColourRGBA = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
//					break;
//			}
//
//			glm::vec3 delta = endKeyFrame.rotation - startKeyFrame.rotation;
//			glm::quat startQuat = glm::quat(startKeyFrame.rotation);
//			glm::quat endQuat = glm::quat(endKeyFrame.rotation);
//			glm::quat offset = glm::slerp(startQuat, endQuat, result);
//			glm::vec3 euler = glm::eulerAngles(offset);
//			affectedMesh->setRotationFromEuler(euler);
//		}
//		
//	}
//#pragma endregion
//
//#pragma region SCALE
//	if (anim->scaleKeyFrames.size() == 1)
//	{
//		affectedMesh->setUniformDrawScale(anim->scaleKeyFrames[0].scale.x);
//	}
//	else
//	{
//		// Find our position keyframes (Start and End)
//		int KeyFrameEndIndex = 0;
//		for (; KeyFrameEndIndex < anim->scaleKeyFrames.size(); KeyFrameEndIndex++)
//		{
//			if (anim->scaleKeyFrames[KeyFrameEndIndex].time > time)
//			{
//				break;
//			}
//		}
//
//		if (KeyFrameEndIndex >= anim->scaleKeyFrames.size())
//		{
//			// we are at or past the last key frame use the last keyframe only
//			affectedMesh->setUniformDrawScale(anim->scaleKeyFrames[KeyFrameEndIndex - 1].scale.x);
//			//return;
//		}
//		else
//		{
//			int KeyFrameStartIndex = KeyFrameEndIndex - 1;
//
//			ScaleKeyFrame startKeyFrame = anim->scaleKeyFrames[KeyFrameStartIndex];
//			ScaleKeyFrame endKeyFrame = anim->scaleKeyFrames[KeyFrameEndIndex];
//
//			float percent = (time - startKeyFrame.time) / (endKeyFrame.time - startKeyFrame.time);
//			float result = 0.f;
//
//			// Check if we are at the end of frame
//			if (percent > 0.99f)
//			{
//				if (startKeyFrame.callback)
//				{
//					startKeyFrame.callback();
//				}
//			}
//
//			switch (endKeyFrame.easingType)
//			{
//				case EasingType::Linear:
//					result = percent;
//					break;
//				case EasingType::SineEaseIn:
//					result = glm::sineEaseIn(percent);
//					break;
//				case EasingType::SineEaseOut:
//					result = glm::sineEaseOut(percent);
//					break;
//				case EasingType::SineEaseInOut:
//					result = glm::sineEaseInOut(percent);
//					break;
//			}
//
//			glm::vec3 delta = endKeyFrame.scale - startKeyFrame.scale;
//			affectedMesh->setUniformDrawScale(startKeyFrame.scale.x + delta.x * result);
//		}
//		
//	}
//#pragma endregion

}

void AnimationSystem::UpdateBoneTransforms(cMesh* mesh, Node& node, float dt)
{
	std::string nodeName = node.Name;

	AnimationInfo info = mesh->modelDrawInfo.Animations[currentAnimationIndex];
	std::map<std::string, NodeAnimation*>::iterator animIt = info.NodeAnimations.find(nodeName);
	

	if (animIt != info.NodeAnimations.end())
	{
		NodeAnimation* nodeAnimInfo = animIt->second;

		glm::mat4 boneTransform = InterpolateNodeTransforms(nodeAnimInfo, dt);
		
		mesh->boneTransformations[nodeName] = boneTransform;
	}

	for (Node* child : node.Children)
	{
		UpdateBoneTransforms(mesh, *child, dt);
	}
}

glm::vec3 AnimationSystem::InterpolatePositions(std::vector<PositionKeyFrame> positions,
	float dt)
{
	if (positions.size() == 1)
	{
		return positions[0].position;
	}
	// Find our position keyframes (Start and End)
	int KeyFrameEndIndex = 0;
	for (; KeyFrameEndIndex < positions.size(); KeyFrameEndIndex++)
	{
		if (positions[KeyFrameEndIndex].time > dt)
		{
			break;
		}
	}

	if (KeyFrameEndIndex >= positions.size())
	{
		// we are at or past the last key frame use the last keyframe only
		return positions[KeyFrameEndIndex - 1].position;
		//return;
	}


	int KeyFrameStartIndex = KeyFrameEndIndex - 1;

	PositionKeyFrame startKeyFrame = positions[KeyFrameStartIndex];
	PositionKeyFrame endKeyFrame = positions[KeyFrameEndIndex];

	float percent = (dt - startKeyFrame.time) / (endKeyFrame.time - startKeyFrame.time);
	float result = 0.f;

	// Check if we are at the end of frame
	if (percent > 0.99f)
	{
		if (startKeyFrame.callback)
		{
			startKeyFrame.callback();
		}
	}

	switch (endKeyFrame.easingType)
	{
		case EasingType::Linear:
			result = percent;
			break;

		case EasingType::SineEaseIn:
			result = glm::sineEaseIn(percent);
			break;

		case EasingType::SineEaseOut:
			result = glm::sineEaseOut(percent);
			break;

		case EasingType::SineEaseInOut:
			result = glm::sineEaseInOut(percent);
			break;
	}

	glm::vec3 delta = endKeyFrame.position - startKeyFrame.position;
	return startKeyFrame.position + delta * result;
}

glm::vec3 AnimationSystem::InterpolateScales(std::vector<ScaleKeyFrame> scales, float dt)
{
	
	if (scales.size() == 1)
	{
		return scales[0].scale;
	}
	// Find our position keyframes (Start and End)
	int KeyFrameEndIndex = 0;
	float time = dt;
	for (; KeyFrameEndIndex < scales.size(); KeyFrameEndIndex++)
	{
		if (scales[KeyFrameEndIndex].time > time)
		{
			break;
		}
	}

	if (KeyFrameEndIndex >= scales.size())
	{
		return scales[KeyFrameEndIndex - 1].scale;
	}
	int KeyFrameStartIndex = KeyFrameEndIndex - 1;

	ScaleKeyFrame startKeyFrame = scales[KeyFrameStartIndex];
	ScaleKeyFrame endKeyFrame = scales[KeyFrameEndIndex];

	float percent = (time - startKeyFrame.time) / (endKeyFrame.time - startKeyFrame.time);
	float result = 0.f;

	// Check if we are at the end of frame
	if (percent > 0.99f)
	{
		if (startKeyFrame.callback)
		{
			startKeyFrame.callback();
		}
	}

	switch (endKeyFrame.easingType)
	{
		case EasingType::Linear:
			result = percent;
			break;
		case EasingType::SineEaseIn:
			result = glm::sineEaseIn(percent);
			break;
		case EasingType::SineEaseOut:
			result = glm::sineEaseOut(percent);
			break;
		case EasingType::SineEaseInOut:
			result = glm::sineEaseInOut(percent);
			break;
	}

	glm::vec3 delta = endKeyFrame.scale - startKeyFrame.scale;

	return startKeyFrame.scale + delta * result;

}

glm::quat AnimationSystem::InterpolateRotations(std::vector<RotationKeyFrame> rotations, float dt)
{
	if (rotations.size() == 1)
	{
		return glm::quat(rotations[0].rotation);
	}

	float time = dt;

	// Find our position keyframes (Start and End)
	int KeyFrameEndIndex = 0;
	for (; KeyFrameEndIndex < rotations.size(); KeyFrameEndIndex++)
	{
		if (rotations[KeyFrameEndIndex].time > time)
		{
			break;
		}
	}

	if (KeyFrameEndIndex >= rotations.size())
	{
		// we are at or past the last key frame use the last keyframe only
		return glm::quat(rotations[KeyFrameEndIndex - 1].rotation);
	}
	int KeyFrameStartIndex = KeyFrameEndIndex - 1;

	RotationKeyFrame startKeyFrame = rotations[KeyFrameStartIndex];
	RotationKeyFrame endKeyFrame = rotations[KeyFrameEndIndex];

	float percent = (time - startKeyFrame.time) / (endKeyFrame.time - startKeyFrame.time);
	float result = 0.f;

	// Check if we are at the end of frame
	if (percent > 0.99f)
	{
		if (startKeyFrame.callback)
		{
			startKeyFrame.callback();
		}
	}

	switch (endKeyFrame.easingType)
	{
		case EasingType::Linear:
			result = percent;
			break;
		case EasingType::SineEaseIn:
			result = glm::sineEaseIn(percent);
			break;
		case EasingType::SineEaseOut:
			result = glm::sineEaseOut(percent);
			break;
		case EasingType::SineEaseInOut:
			result = glm::sineEaseInOut(percent);
			break;
	}

	glm::vec3 delta = endKeyFrame.rotation - startKeyFrame.rotation;
	glm::quat startQuat = glm::quat(startKeyFrame.rotation);
	glm::quat endQuat = glm::quat(endKeyFrame.rotation);
	glm::quat offset = glm::slerp(startQuat, endQuat, result);

	return glm::quat(offset);
}

Animation* AnimationSystem::GetAnimation(AnimationInfo& info, std::string nodeName)
{
	Animation* anim = new Animation(info.AnimationName, nullptr);
	anim->positionKeyFrames = info.NodeAnimations[nodeName]->PositionKeys;
	anim->rotationKeyFrames = info.NodeAnimations[nodeName]->RotationKeys;
	anim->scaleKeyFrames = info.NodeAnimations[nodeName]->ScalingKeys;
	return anim;
}

glm::mat4 AnimationSystem::InterpolateNodeTransforms(NodeAnimation* nodeAnim, float dt)
{
	glm::vec3 position = InterpolatePositions(nodeAnim->PositionKeys, dt);
	glm::quat rotation = InterpolateRotations(nodeAnim->RotationKeys, dt);
	glm::vec3 scale = InterpolateScales(nodeAnim->ScalingKeys, dt);

	glm::mat4 matModel = glm::mat4(1.0f);

	glm::mat4 translate = glm::translate(glm::mat4(1.0f), position);
	glm::mat4 rotate = glm::mat4(rotation);
	glm::mat4 scaled = glm::scale(glm::mat4(1.0f), scale);

	matModel = matModel * translate;
	matModel = matModel * rotate;
	matModel = matModel * scaled;

	return matModel;
}


