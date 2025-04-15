// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "Enum/PlayerState_enum.h"
#include "BurstingColdCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class ABurstingColdCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	/**crouched Input Action**/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* CrouchAction;

    /*run Input Action*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* RunAction;

public:
	ABurstingColdCharacter();
	

protected:

	//切换行走状态
	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	/** Called for crouching input */
	void Crouch();
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = motion, meta = (AllowPrivateAccess = "true"))//玩家状态
	bool isCropuching = false;

	/** Called for running input */
	void AddSpeed();
	void SubSpeed();
	EPlayerState_Move PlayerState = EPlayerState_Move::EPlayerState_Walk;
	int SetPlayer_Move_State(EPlayerState_Move state);
	int moveNum = 0;

			

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay();

	virtual void Tick(float DeltaTime) override;

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	//获取玩家状态序号
	FORCENOINLINE int GetPlayer_Move_State() { return moveNum; }

	//motionTraject
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = motion, meta = (AllowPrivateAccess = "true"))
	class UCharacterTrajectoryComponent* CharacterTrajectoryComponent;

	public: 
	UFUNCTION(BlueprintCallable, Category = "motion")
	FORCENOINLINE UCharacterTrajectoryComponent* GetTrajectoryComponent(){return CharacterTrajectoryComponent;}
	//蹲下
	UFUNCTION(BlueprintCallable, Category = "motion")
	FORCENOINLINE bool GetCrouchStatus() {return isCropuching;}
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "motion")
	class UCurveFloat* AddCropuchCurve;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "motion")
	class UTimelineComponent* CropuchTimeline;
	UFUNCTION()
	void AddCameraLegacy(float Value);

	//

};

