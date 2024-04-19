#include "cPlayerAnimations.h"

cPlayerAnimations::cPlayerAnimations()
{
	lerpTime = 0.0f;
	lerpIncrementTime = 0.0f;
	stopBlending = false;
	previousState = PlayerState::IDLE;
	currentPlayerState = PlayerState::IDLE;
}

cPlayerAnimations::~cPlayerAnimations()
{
}

void cPlayerAnimations::animInit(cMesh* playerMesh)
{

	m_PlayerMesh = playerGameObject->GetComponent<cMesh>();
	animationSystem = &playerGameObject->AddComponent<AnimationSystem>();

	animationSystem->SetMesh(m_PlayerMesh);

	LoadAnimations();

	currentPlayerState = PlayerState::IDLE;
	animationSystem->animationSpeed = 0.1f;
	animationSystem->SetBlendIndex(0, (int)currentPlayerState);
	animationSystem->SetBlendWeight(1.0f);

}


void cPlayerAnimations::LoadAnimations()
{
	////load animations
	animationSystem->LoadAnimationFromFile("A_Pistol_Idle.dae");
	animationSystem->LoadAnimationFromFile("A_Pistol_Walk.dae");
	animationSystem->LoadAnimationFromFile("A_Pistol_Walk_Backward.dae");
	animationSystem->LoadAnimationFromFile("A_Pistol_Strafe_Left.dae");
	animationSystem->LoadAnimationFromFile("A_Pistol_Strafe_Right.dae");
	animationSystem->LoadAnimationFromFile("A_Pistol_Jump.dae");
	animationSystem->LoadAnimationFromFile("A_Falling_Back_Death.dae");

}

float cPlayerAnimations::lerp(float from, float to, float t)
{
	return from + t * (to - from); 
}


void cPlayerAnimations::BlendTo(PlayerState playerState, float deltaTime)
{


	if (stopBlending)
	{
		return;
	}

	float lerpedValue = 0.0f;

	lerpTime += deltaTime;
	lerpedValue = lerp(0.0f, 1.0f, lerpTime * 5.0f);
	animationSystem->SetBlendWeight(lerpedValue);


	if (lerpedValue >= 1.0f)
	{
		lerpTime = 0.0f;
		stopBlending = true;
		previousState = currentPlayerState;
	}
}

void cPlayerAnimations::Update(float deltaTime)
{
	BlendTo(currentPlayerState, deltaTime);
}

void cPlayerAnimations::SetState(PlayerState playerState)
{
	if (currentPlayerState != playerState)
	{
		currentPlayerState = playerState;
		animationSystem->SetBlendIndex((int)previousState, (int)currentPlayerState);
		stopBlending = false;
	}
}
