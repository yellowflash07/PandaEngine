#pragma once
#include "AnimationSystem.h"
#include "cMesh.h"
#include <map>

class cPlayerAnimations
{
public:
	cPlayerAnimations();
	~cPlayerAnimations();

	void animInit(cMesh* playerMesh);

	void LoadAnimations();

	enum class PlayerState {
		IDLE = 1,
		WALKFRONT = 2,
		WALKBACK = 3,
		WALKLEFT = 4,
		WALKRIGHT = 5,
		JUMP = 6,
		DIE = 7
	};

	void BlendTo(PlayerState playerState, float deltaTime);

	GameObject* playerGameObject;
	AnimationSystem* animationSystem;

private:

	cMesh* m_PlayerMesh;

	float blendWeight = 0.0f;
	float blendIncrement = 1.0f;

	std::map<PlayerState, AnimationInfo> playerAnimStates;

	float lerp(float a, float b, float t);


	PlayerState currentPlayerState;
	bool stopBlending;
	float lerpTime = 0.0f;

};

