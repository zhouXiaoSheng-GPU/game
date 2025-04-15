// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "BurstingCold/BurstingColdCharacter.h"
#include "Player_AnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class BURSTINGCOLD_API UPlayer_AnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaTime) override;

	ABurstingColdCharacter* MyCharacter;//Player

   UPROPERTY(BlueprintReadOnly, Category = "motion")
    class UCharacterTrajectoryComponent* playerTrajectoryComponent;

	UPROPERTY(BlueprintReadOnly, Category = "motion")//速度
	float speed;

	UPROPERTY(BlueprintReadOnly, Category = "motion")//蹲下
	bool isCrouching;

	UPROPERTY(BlueprintReadOnly, Category = "motion")//玩家行走状态
	int moveNum;
	
};
