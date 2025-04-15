// Fill out your copyright notice in the Description page of Project Settings.


#include "Player_AnimInstance.h"
#include "CharacterTrajectoryComponent.h"

void UPlayer_AnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	 MyCharacter = Cast<ABurstingColdCharacter>(TryGetPawnOwner());
}

void UPlayer_AnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	if (MyCharacter) {

		playerTrajectoryComponent = MyCharacter->GetTrajectoryComponent();
		speed = MyCharacter->GetVelocity().Size();
		isCrouching = MyCharacter->GetCrouchStatus();
		moveNum = MyCharacter->GetPlayer_Move_State();

	}
}
