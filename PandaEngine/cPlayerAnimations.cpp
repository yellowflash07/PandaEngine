#include "cPlayerAnimations.h"

cPlayerAnimations::cPlayerAnimations()
{
}

cPlayerAnimations::~cPlayerAnimations()
{
}

void cPlayerAnimations::animInit(cMesh* playerMesh)
{

	m_PlayerMesh = playerGameObject->GetComponent<cMesh>();
	animationSystem = &playerGameObject->AddComponent<AnimationSystem>();
	animationSystem->m_mesh = m_PlayerMesh;

	LoadAnimations();

	currentPlayerState = PlayerState::IDLE;
	animationSystem->animationSpeed = 0.1f;
	animationSystem->SetBlend(0, (int)currentPlayerState, 1.0f);

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
	return from + t * (to - from); // Check for any invisible characters between operators
}


void cPlayerAnimations::BlendTo(PlayerState playerState, float deltaTime)
{

	lerpTime += deltaTime;

	float lerpedValue = lerp(0.0f, 1.0f, lerpTime);

	printf("lerp %f \n", lerpTime);

	animationSystem->SetBlend((int)currentPlayerState, (int)playerState, lerpedValue);

	if (lerpTime >= 1.0f)
	{
		lerpTime = 0.0f;
		currentPlayerState = playerState;
		stopBlending = true;
	}
	else
	{
		stopBlending = false;
	}

	if (!stopBlending)
	{


	}

}