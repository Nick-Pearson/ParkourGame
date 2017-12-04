// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "ParkourGameCharacter.h"

#include "Private/Physics/ConstraintManager.h"
#include "ParkourTypes.h"
#include "Private/Utils/ParkourHelperLibrary.h"

// Engine
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"

//////////////////////////////////////////////////////////////////////////
// AParkourGameCharacter

AParkourGameCharacter::AParkourGameCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	InRagdoll = false;

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)

	ConstraintManager = CreateDefaultSubobject<UConstraintManager>(TEXT("ConstraintManager"));
}


void AParkourGameCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	SkeletalMesh = Cast<USkeletalMeshComponent>(GetComponentByClass(USkeletalMeshComponent::StaticClass()));
}

//////////////////////////////////////////////////////////////////////////
// Input

void AParkourGameCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &AParkourGameCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AParkourGameCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AParkourGameCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AParkourGameCharacter::LookUpAtRate);

	// Ragdoll controls
	PlayerInputComponent->BindAction("RagdollBody", IE_Pressed, this, &AParkourGameCharacter::RagdollBody);
	PlayerInputComponent->BindAction("RagdollArmR", IE_Pressed, this, &AParkourGameCharacter::RagdollArmR);
	PlayerInputComponent->BindAction("RagdollArmL", IE_Pressed, this, &AParkourGameCharacter::RagdollArmL);
	PlayerInputComponent->BindAction("RagdollLegR", IE_Pressed, this, &AParkourGameCharacter::RagdollLegR);
	PlayerInputComponent->BindAction("RagdollLegL", IE_Pressed, this, &AParkourGameCharacter::RagdollLegL);
	PlayerInputComponent->BindAction("RagdollTorso", IE_Pressed, this, &AParkourGameCharacter::RagdollTorso);
	PlayerInputComponent->BindAction("RagdollLegs", IE_Pressed, this, &AParkourGameCharacter::RagdollLegs);
}

void AParkourGameCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	CapsuleToRagdoll();
}

void AParkourGameCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AParkourGameCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AParkourGameCharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AParkourGameCharacter::MoveRight(float Value)
{
	if ( (Controller != NULL) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void AParkourGameCharacter::RagdollBody()
{
	USkeletalMeshComponent* PlayerMesh = GetSkeletalMesh();
	PlayerMesh->SetSimulatePhysics(true);
	InRagdoll = true;
}

void AParkourGameCharacter::RagdollArmR()
{
	USkeletalMeshComponent* PlayerMesh = GetSkeletalMesh();
	PlayerMesh->SetAllBodiesBelowSimulatePhysics(UParkourHelperLibrary::GetRootBoneForBodyPart(EBodyPart::RightArm), true, true);
}

void AParkourGameCharacter::RagdollArmL()
{
	USkeletalMeshComponent* PlayerMesh = GetSkeletalMesh();
	PlayerMesh->SetAllBodiesBelowSimulatePhysics(UParkourHelperLibrary::GetRootBoneForBodyPart(EBodyPart::LeftArm), true, true);
}

void AParkourGameCharacter::RagdollLegR()
{
	USkeletalMeshComponent* PlayerMesh = GetSkeletalMesh();
	PlayerMesh->SetAllBodiesBelowSimulatePhysics(UParkourHelperLibrary::GetRootBoneForBodyPart(EBodyPart::RightLeg), true, true);
}

void AParkourGameCharacter::RagdollLegL()
{
	USkeletalMeshComponent* PlayerMesh = GetSkeletalMesh();
	PlayerMesh->SetAllBodiesBelowSimulatePhysics(UParkourHelperLibrary::GetRootBoneForBodyPart(EBodyPart::LeftLeg), true, true);
}

void AParkourGameCharacter::RagdollTorso()
{
	USkeletalMeshComponent* PlayerMesh = GetSkeletalMesh();
	PlayerMesh->SetAllBodiesBelowSimulatePhysics(UParkourHelperLibrary::GetRootBoneForBodyPart(EBodyPart::Torso), true, true);
}

void AParkourGameCharacter::RagdollLegs()
{
	USkeletalMeshComponent* PlayerMesh = GetSkeletalMesh();
	PlayerMesh->SetAllBodiesBelowSimulatePhysics(UParkourHelperLibrary::GetRootBoneForBodyPart(EBodyPart::RightLeg), true, true);
	PlayerMesh->SetAllBodiesBelowSimulatePhysics(UParkourHelperLibrary::GetRootBoneForBodyPart(EBodyPart::LeftLeg), true, true);
}

void AParkourGameCharacter::CapsuleToRagdoll()
{
	USkeletalMeshComponent* PlayerMesh = GetSkeletalMesh();
	if (InRagdoll == true) {
		FVector SocketLocation = PlayerMesh->GetSocketLocation(UParkourHelperLibrary::GetRootBoneForBodyPart(EBodyPart::Pelvis));
		UCapsuleComponent* Capsule = GetCapsuleComponent();
		Capsule->SetWorldLocation(SocketLocation);
	}
}