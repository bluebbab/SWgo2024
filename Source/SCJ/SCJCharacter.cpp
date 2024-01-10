// Copyright Epic Games, Inc. All Rights Reserved.

#include "SCJCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Kismet/GameplayStatics.h"
#include "InteractInterface.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// ASCJCharacter

ASCJCharacter::ASCJCharacter()
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
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
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
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	HideEffectPoint = CreateDefaultSubobject<USceneComponent>(TEXT("EffectPoint"));
}

void ASCJCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	Tags.Add("UnHide");
	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

APlayerController* ASCJCharacter::GetPlayerController() const
{
	return UGameplayStatics::GetPlayerController(GetWorld(), 0);
}

//////////////////////////////////////////////////////////////////////////
// Input

void ASCJCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ASCJCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASCJCharacter::Look);

		// Hiding
		EnhancedInputComponent->BindAction(HideAction, ETriggerEvent::Triggered, this, &ASCJCharacter::Hide);

		// Interact system
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Triggered, this, &ASCJCharacter::Interact);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ASCJCharacter::Move(const FInputActionValue& Value)
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

void ASCJCharacter::Look(const FInputActionValue& Value)
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

void ASCJCharacter::Hide()
{
	UCharacterMovementComponent* MovementComp = GetCharacterMovement();
	if (FlowControl == true)
	{
		GetMesh()->SetSkeletalMeshAsset(SkeletalBox);
		MovementComp->MaxWalkSpeed = 0;
		MovementComp->MinAnalogWalkSpeed = 0;
		MovementComp->bOrientRotationToMovement = false;
		Tags.Add("Hide");
		Tags.Remove("UnHide");
		GetCapsuleComponent()->SetCapsuleRadius(70.0f);
		if (HideEffect)
		{
			UNiagaraComponent* NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAttached(HideEffect,
				HideEffectPoint,
				NAME_None,
				GetActorLocation(),
				GetActorRotation(),
				EAttachLocation::Type::KeepRelativeOffset,
				true);
		}
		FlowControl = false;
	}
	else
	{
		GetMesh()->SetSkeletalMeshAsset(SkeletalChar);
		MovementComp->MaxWalkSpeed = 500;
		MovementComp->MinAnalogWalkSpeed = 20.0f;
		MovementComp->bOrientRotationToMovement = true;
		Tags.Add("UnHide");
		Tags.Remove("Hide");
		GetCapsuleComponent()->SetCapsuleRadius(35.0f);
		if (HideEffect)
		{
			UNiagaraComponent* NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAttached(HideEffect,
				HideEffectPoint,
				NAME_None,
				GetActorLocation(),
				GetActorRotation(),
				EAttachLocation::Type::KeepRelativeOffset,
				true);
		}
		FlowControl = true;
	}
}

void ASCJCharacter::Interact()
{
	FVector Start = GetActorLocation();
	FVector End = GetActorLocation();
	FHitResult HitResult;
	FCollisionQueryParams TraceParams;
	TraceParams.AddIgnoredActor(this);

	bool bIsHit = GetWorld()->SweepSingleByChannel(HitResult,
		Start,
		End,
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel1,
		FCollisionShape::MakeSphere(100.f),
		TraceParams);

	if (bIsHit)
	{
		UE_LOG(LogTemp, Warning, TEXT("Hit!"));
		AActor* HitActor = HitResult.GetActor();
		IInteractInterface* Interface = Cast<IInteractInterface>(HitActor);
		if (Interface)
		{
			Interface->Interact();
		}
	}
}