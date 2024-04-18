#include "AnimationSystem.h"
#include <glm/gtx/easing.hpp>
#include <iostream>
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags
#include <glm/gtc/matrix_transform.hpp>

AnimationSystem::AnimationSystem()
{
	m_isPaused = false;
	animationSpeed = 1.0f;
	frameCount = 0.0f;
	currentAnimationIndex = 0;
	m_mesh = nullptr;
}

AnimationSystem::~AnimationSystem()
{
}

void AnimationSystem::Update(float dt)
{
	// Update all animations
	//for (Animation* anim : m_animations)
	//{
	//	// Skip if paused
	//	if (m_isPaused)
	//	{
	//		continue;
	//	}
	//	//std::cout << dt << std::endl;
	//	UpdateAnimation(anim, dt);
	//	UpdateBoneTransforms(anim->mesh, *anim->mesh->modelDrawInfo.RootNode, dt);
	//}

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

void AnimationSystem::UpdateSkeleton(cMesh* mesh, float dt)
{
	if (m_mesh == nullptr)
	{
		m_mesh = mesh; 
	}
	//m_drawInfo = &m_mesh->modelDrawInfo[0];
	for(int i = 0; i < m_drawInfos.size(); i++)
	{
		sModelDrawInfo* pDrawInfo = m_drawInfos[i];

		//sModelDrawInfo* drawInfo = &mesh->modelDrawInfo[0];
		if (pDrawInfo->Animations.empty())
		{
			return;
		}

		AnimationInfo info = pDrawInfo->Animations[currentAnimationIndex];

		frameCount += dt * info.TicksPerSecond * animationSpeed;
		if (frameCount > info.Duration)
		{
			frameCount = 0.0f;
		}

		if (isBlending)
		{
			BlendBoneTransforms(pDrawInfo, *pDrawInfo->RootNode, frameCount, blendIndexA, blendIndexB, blendWeight);
		}
		else
		{
			UpdateBoneTransforms(pDrawInfo, *pDrawInfo->RootNode, frameCount);
		}
	}
	

}

void AnimationSystem::UpdateBoneTransforms(sModelDrawInfo* drawInfo, Node& node, float dt)
{

	AnimationInfo info = drawInfo->Animations[currentAnimationIndex];
	
	std::string nodeName = node.Name;
	
	std::map<std::string, NodeAnimation*>::iterator animIt = info.NodeAnimations.find(nodeName);
	
	
	if (animIt != info.NodeAnimations.end())
	{
		NodeAnimation* nodeAnimInfo = animIt->second;
	
		glm::mat4 boneTransform = InterpolateNodeTransforms(nodeAnimInfo, dt);
		
		drawInfo->boneTransformations[nodeName] = boneTransform;
	}
	
	for (Node* child : node.Children)
	{
		UpdateBoneTransforms(drawInfo, *child, dt);
	}
}

void AnimationSystem::BlendBoneTransforms(sModelDrawInfo* drawInfo, Node& node, float dt, int animationIndexA, int animationIndexB, float blendWeight)
{
	// Ensure weights are between 0 and 1
	blendWeight = glm::clamp(blendWeight, 0.0f, 1.0f);

	isBlending = true;

	if (blendWeight <= 0.0f)
	{
		currentAnimationIndex = animationIndexA ;
		isBlending = false;
		return;
	}

	if (blendWeight >= 1.0f)
	{
		currentAnimationIndex = animationIndexB;
		isBlending = false;
		//UpdateBoneTransforms(drawInfo, node, dt);
		return;
	}

	// Get animation information for both animations
	AnimationInfo infoA = drawInfo->Animations[animationIndexA];
	AnimationInfo infoB = drawInfo->Animations[animationIndexB];

	std::string nodeName = node.Name;

	// Find node animations in both info structs
	std::map<std::string, NodeAnimation*>::iterator animItA = infoA.NodeAnimations.find(nodeName);
	std::map<std::string, NodeAnimation*>::iterator animItB = infoB.NodeAnimations.find(nodeName);

	// If animations exist for the node in both A and B
	if (animItA != infoA.NodeAnimations.end() && animItB != infoB.NodeAnimations.end())
	{
		NodeAnimation* nodeAnimInfoA = animItA->second;
		NodeAnimation* nodeAnimInfoB = animItB->second;

		// Interpolate transforms for each animation
		glm::mat4 boneTransformA = InterpolateNodeTransforms(nodeAnimInfoA, dt);
		glm::mat4 boneTransformB = InterpolateNodeTransforms(nodeAnimInfoB, dt);

		// Blend the interpolated transforms based on weight
		glm::mat4 blendedTransform = blendWeight * boneTransformB + (1.0f - blendWeight) * boneTransformA;


		// Update bone transformations in drawInfo
		drawInfo->boneTransformations[nodeName] = blendedTransform;
	}

	// Recursively update child nodes
	for (Node* child : node.Children)
	{
		BlendBoneTransforms(drawInfo, *child, dt, animationIndexA, animationIndexB, blendWeight);
	}
}

void AnimationSystem::SetBlendIndex(int indexA, int indexB)
{
	if (isBlending) return;
	blendIndexA = indexA;
	blendIndexB = indexB;
	isBlending = true;
}

void AnimationSystem::SetBlendWeight(float weight)
{
	blendWeight = weight;
}

void AnimationSystem::Render()
{
	ImGui::BeginChild("Animation", ImVec2(0, 150));
	ImGui::SeparatorText("Animation");
	
	ImGui::SetNextItemWidth(40);
	ImGui::InputFloat("Speed", &animationSpeed);

	//if (m_mesh != nullptr)
	//{
	//	if (!m_mesh->modelDrawInfo.Animations.empty())
	//	{
	//		ImGui::Text("Number of Animations: %d", m_mesh->modelDrawInfo.Animations.size());
	//		if (ImGui::InputInt("Current Animation", &currentAnimationIndex))
	//		{
	//			m_mesh->modelDrawInfo.RootNode = m_mesh->modelDrawInfo.Animations[currentAnimationIndex].RootNode;
	//		}
	//	}
	//}	
	//

	ImGui::Text("Number of Animations: %d", m_mesh->modelDrawInfo[0].Animations.size());
	if (ImGui::InputInt("Current Animation", &currentAnimationIndex))
	{
		if (currentAnimationIndex >= m_mesh->modelDrawInfo[0].Animations.size())
		{
			currentAnimationIndex = 0;
		}
	}
	ImGui::Text("Add Animation");
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Model_DND"))
		{
			const char* payload_n = (const char*)payload->Data;
			LoadAnimationFromFile(payload_n);
		}
		ImGui::EndDragDropTarget();
	}

	ImGui::Separator();
	ImGui::EndChild();
}

glm::vec3 AnimationSystem::InterpolatePositions(std::vector<PositionKeyFrame> positions,
	float dt)
{
	if (positions.size() == 1)
	{
		return positions[0].position;
	}
	// Find our position keyframes (Start and End)
	int KeyFrameEndIndex = 1;
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
	int KeyFrameEndIndex = 1;
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
		return rotations[0].rotation;
	}

	float time = dt;

	// Find our position keyframes (Start and End)
	int KeyFrameEndIndex = 1;
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
		return rotations[KeyFrameEndIndex - 1].rotation;
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

	glm::quat offset = glm::slerp(startKeyFrame.rotation, endKeyFrame.rotation, result);

	return offset;
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


void AnimationSystem::LoadAnimationFromFile(std::string fileName)
{
	Assimp::Importer importer;
	std::string filePath = "../Assets/Models/" + fileName;
	const aiScene* scene = importer.ReadFile(filePath, aiProcess_ValidateDataStructure | aiProcess_GenNormals | aiProcess_Triangulate);

	std::string errorString = importer.GetErrorString();
	if (!errorString.empty())
	{
		std::cout << "Error: " << importer.GetErrorString() << std::endl;
	}

	if (!scene)
	{
		// Error loading the model
		// Handle the error as needed
		return;
	}

	if (scene->HasAnimations())
	{
		for (unsigned int i = 0; i < scene->mNumAnimations; i++)
		{
			aiAnimation* pAnimation = scene->mAnimations[i];
			AnimationInfo animInfo;
			animInfo.AnimationName = pAnimation->mName.C_Str();
			animInfo.Duration = (float)pAnimation->mDuration;
			animInfo.TicksPerSecond = (float)pAnimation->mTicksPerSecond;
			for (unsigned int j = 0; j < pAnimation->mNumChannels; j++)
			{
				aiNodeAnim* pNodeAnim = pAnimation->mChannels[j];
				NodeAnimation* nodeAnim = new NodeAnimation(pNodeAnim->mNodeName.C_Str());
				for (unsigned int k = 0; k < pNodeAnim->mNumPositionKeys; k++)
				{
					glm::vec3 pos = glm::vec3(pNodeAnim->mPositionKeys[k].mValue.x,
						pNodeAnim->mPositionKeys[k].mValue.y,
						pNodeAnim->mPositionKeys[k].mValue.z);
					PositionKeyFrame keyFrame(pos, (float)pNodeAnim->mPositionKeys[k].mTime);
					nodeAnim->PositionKeys.push_back(keyFrame);
				}
				for (unsigned int k = 0; k < pNodeAnim->mNumRotationKeys; k++)
				{
					glm::quat rotQuat = glm::quat((float)pNodeAnim->mRotationKeys[k].mValue.w,
						(float)pNodeAnim->mRotationKeys[k].mValue.x,
						(float)pNodeAnim->mRotationKeys[k].mValue.y,
						(float)pNodeAnim->mRotationKeys[k].mValue.z);

					RotationKeyFrame keyFrame(rotQuat, (float)pNodeAnim->mRotationKeys[k].mTime);
					nodeAnim->RotationKeys.push_back(keyFrame);
				}
				for (unsigned int k = 0; k < pNodeAnim->mNumScalingKeys; k++)
				{
					glm::vec3 scale = glm::vec3(pNodeAnim->mScalingKeys[k].mValue.x,
						pNodeAnim->mScalingKeys[k].mValue.y,
						pNodeAnim->mScalingKeys[k].mValue.z);
					ScaleKeyFrame keyFrame(scale, (float)pNodeAnim->mScalingKeys[k].mTime);
					nodeAnim->ScalingKeys.push_back(keyFrame);
				}

				animInfo.NodeAnimations.insert(std::pair<std::string, NodeAnimation*>(nodeAnim->Name, nodeAnim));
			}

			for (int i = 0; i < m_mesh->modelDrawInfo.size(); i++)
			{
				sModelDrawInfo* drawInfo = &m_mesh->modelDrawInfo[i];
				drawInfo->Animations.push_back(animInfo);
			}
			//m_mesh->modelDrawInfo.Animations.push_back(info);
			printf("Added Animation: %s\n", animInfo.AnimationName.c_str());
		}
	}
}

void AnimationSystem::AttachObjectToBone(std::string boneName, TransformComponent* transform)
{

	// Attach an object to a bone
	// Find the bone in the skeleton

	for (int j = 0; j < m_mesh->modelDrawInfo.size(); j++)
	{
		sModelDrawInfo* drawInfo = &m_mesh->modelDrawInfo[j];
		for (int i = 0; i < drawInfo->vecBoneInfo.size(); i++)
		{
			BoneInfo* boneInfo = &drawInfo->vecBoneInfo[i];
			if (boneInfo->boneName == boneName)
			{
				transform->parentTransform = boneInfo->GlobalTransformation;
			}
		}
	}

	

}

void AnimationSystem::SetMesh(cMesh* mesh)
{
	if (this->m_mesh == nullptr)
	{
		this->m_mesh = mesh;

		for (int i = 0; i < mesh->modelDrawInfo.size(); i++)
		{
			this->m_drawInfos.push_back(&mesh->modelDrawInfo[i]);
			//m_drawInfos.push_back()
		}

		//this->m_drawInfo = &m_mesh->modelDrawInfo[0];
	}
}


