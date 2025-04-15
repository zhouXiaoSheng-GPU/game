// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerState_enum.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class EPlayerState_Move : uint8
{
	EPlayerState_Walk UMETA(DisplayName = "Walk"),
	EPlayerState_Run UMETA(DisplayName = "Run"),
	EPlayerState_Crouch UMETA(DisplayName = "Crouch"),
	
};
