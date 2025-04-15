// Copyright Epic Games, Inc. All Rights Reserved.

#include "BurstingColdCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "CharacterTrajectoryComponent.h"
#include "Components/TimelineComponent.h"
#include "TimerManager.h"


DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
ABurstingColdCharacter::ABurstingColdCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 200;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false;
	CropuchTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("CropuchTimeline"));
	CharacterTrajectoryComponent = CreateDefaultSubobject<UCharacterTrajectoryComponent>(TEXT("CharacterTrajectoryComponent"));
	// Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void ABurstingColdCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
}

void ABurstingColdCharacter::Tick(float DeltaTime)
{
	if (GetVelocity().Size() > 300.f||GetVelocity().Size() <=1.f) {
		GetCharacterMovement()->bOrientRotationToMovement = true;
		GetCharacterMovement()->bUseControllerDesiredRotation = false;
	}
	else {
		GetCharacterMovement()->bOrientRotationToMovement = false;
		GetCharacterMovement()->bUseControllerDesiredRotation = true;
	}
	
	
}

//////////////////////////////////////////////////////////////////////////
// Input

void ABurstingColdCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ABurstingColdCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ABurstingColdCharacter::Look);

		// Crouching
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Triggered, this, &ABurstingColdCharacter::Crouch);

		//Run
		EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Triggered, this, &ABurstingColdCharacter::AddSpeed);
		EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Completed, this, &ABurstingColdCharacter::SubSpeed);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ABurstingColdCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ABurstingColdCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

//蹲下
void ABurstingColdCharacter::Crouch()
{
	if (PlayerState!=EPlayerState_Move::EPlayerState_Run) {
	isCropuching = !isCropuching;
	FOnTimelineFloat CrouchTimelineFloat;
	if (isCropuching) {
		if (CropuchTimeline) {
			CrouchTimelineFloat.BindUFunction(this, "AddCameraLegacy");
			CropuchTimeline->AddInterpFloat(AddCropuchCurve, CrouchTimelineFloat);
			CropuchTimeline->Play();
			moveNum = SetPlayer_Move_State(EPlayerState_Move::EPlayerState_Crouch);
		}
	}
	else {
		if (CropuchTimeline) {
			CropuchTimeline->Reverse();
			moveNum = SetPlayer_Move_State(EPlayerState_Move::EPlayerState_Walk);
		}
	}
	}
	
}

//奔跑
void ABurstingColdCharacter::AddSpeed()
{
	GetCharacterMovement()->MaxWalkSpeed = 400.f;
	moveNum = SetPlayer_Move_State(EPlayerState_Move::EPlayerState_Run);
}
void ABurstingColdCharacter::SubSpeed()
{
	GetCharacterMovement()->MaxWalkSpeed = 200.f;
	moveNum = SetPlayer_Move_State(EPlayerState_Move::EPlayerState_Walk);
}

int ABurstingColdCharacter::SetPlayer_Move_State(EPlayerState_Move state)
{
	switch (state) {
        case  EPlayerState_Move::EPlayerState_Walk:
			PlayerState = EPlayerState_Move::EPlayerState_Walk;
			isCropuching = false;
			return 0;
			break;
	    case EPlayerState_Move::EPlayerState_Run:
			PlayerState = EPlayerState_Move::EPlayerState_Run;
			isCropuching = false;
			CropuchTimeline->Reverse();
			return 1;
			break;
	    case EPlayerState_Move::EPlayerState_Crouch:
			PlayerState = EPlayerState_Move::EPlayerState_Crouch;
			isCropuching = true;
			return 2;
			break;
	}
	
	return 0;
}

void ABurstingColdCharacter::AddCameraLegacy(float Value)
{
	float CameraBoomLong = FMath::Lerp(170, 255, Value);
	CameraBoom->SocketOffset.Z = FMath::Lerp(68, 34, Value);
	CameraBoom->TargetArmLength = CameraBoomLong;
}
