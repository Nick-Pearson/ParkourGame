// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "ParkourGameCharacter.h"

#include "Private/Physics/ConstraintManager.h"
#include "Private/Utils/ParkourHelperLibrary.h"

// Engine
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Private/ParkourInteractiveZone.h"
#include "ParkourMesh.h"
#include "UnrealNetwork.h"

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

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)

	ConstraintManager = CreateDefaultSubobject<UConstraintManager>(TEXT("ConstraintManager"));
}

void AParkourGameCharacter::BeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	AParkourMesh* Other = Cast<AParkourMesh>(OtherActor);

	if (Other != nullptr) 
	{
		NearbyParkourObjects.Add(Other);
	}

}

void AParkourGameCharacter::BeginPlay()
{
	Super::BeginPlay();
	OnActorBeginOverlap.AddDynamic(this, &AParkourGameCharacter::BeginOverlap);
	OnActorEndOverlap.AddDynamic(this, &AParkourGameCharacter::EndOverlap);
}

void AParkourGameCharacter::EndPlay(EEndPlayReason::Type Reason)
{
	Super::EndPlay(Reason);
	OnActorBeginOverlap.RemoveDynamic(this, &AParkourGameCharacter::BeginOverlap);
	OnActorEndOverlap.RemoveDynamic(this, &AParkourGameCharacter::EndOverlap);
}

void AParkourGameCharacter::EndOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	AParkourMesh* Other = Cast<AParkourMesh>(OtherActor);

	if (Other != nullptr)
	{
		NearbyParkourObjects.Remove(Other);
	}

}

AParkourMesh* AParkourGameCharacter::GetNearestParkourObject()
{
	float MinDistSq = -1.0f;
	AParkourMesh* NearestParkourObject = nullptr;
	for (auto& PMesh : NearbyParkourObjects) {
		float DistSq = (this->GetActorLocation() - PMesh->GetActorLocation()).SizeSquared();
		if ((DistSq > MinDistSq) || (MinDistSq == 1.0f)) 
		{
			MinDistSq = DistSq;
			NearestParkourObject = PMesh;
		}

	}
	return NearestParkourObject;
}

void AParkourGameCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	SkeletalMesh = Cast<USkeletalMeshComponent>(GetComponentByClass(USkeletalMeshComponent::StaticClass()));
}

void AParkourGameCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AParkourGameCharacter, m_RagdollState);
}

void AParkourGameCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	CapsuleToRagdoll();
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

FVector AParkourGameCharacter::GetParkourHandTarget(EHandSideEnum handSide)
{
	FVector HandPos;
	AParkourMesh* ClosestParkourMesh;
	if (handSide == EHandSideEnum::HS_Left) 
	{
		HandPos = GetMesh()->GetSocketLocation("Hand_lSocket");
	}
	else
	{
		HandPos = GetMesh()->GetSocketLocation("Hand_rSocket");
	}
	
	ClosestParkourMesh = GetNearestParkourObject();

	if (ClosestParkourMesh == nullptr) {
		return HandPos;
	}

	FVector LineStart = ClosestParkourMesh->InteractiveZonePointer->ZoneCoord1 + ClosestParkourMesh->GetActorLocation();
	FVector LineEnd = ClosestParkourMesh->InteractiveZonePointer->ZoneCoord2 + ClosestParkourMesh->GetActorLocation();

	FBox box(LineEnd, LineStart);
	return box.GetClosestPointTo(HandPos);
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

void AParkourGameCharacter::RagdollBody()
{
	SetFullRagdoll(true);
}

void AParkourGameCharacter::RagdollArmR()
{
	SetRagdollOnBodyPart(EBodyPart::RightArm, true);
}

void AParkourGameCharacter::RagdollArmL()
{
	SetRagdollOnBodyPart(EBodyPart::LeftArm, true);
}

void AParkourGameCharacter::RagdollLegR()
{
	SetRagdollOnBodyPart(EBodyPart::RightLeg, true);
}

void AParkourGameCharacter::RagdollLegL()
{
	SetRagdollOnBodyPart(EBodyPart::LeftLeg, true);
}

void AParkourGameCharacter::RagdollTorso()
{
	SetRagdollOnBodyPart(EBodyPart::Torso, true);
}

void AParkourGameCharacter::RagdollLegs()
{
	SetRagdollOnBodyPart(EBodyPart::RightLeg, true);
	SetRagdollOnBodyPart(EBodyPart::LeftLeg, true);
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

bool AParkourGameCharacter::SetRagdollOnBodyPart_Validate(EBodyPart Part, bool bNewRagdoll) { return true; }
void AParkourGameCharacter::SetRagdollOnBodyPart_Implementation(EBodyPart Part, bool bNewRagdoll)
{
	if (!ensureMsgf(Part != EBodyPart::MAX, TEXT("[AParkourGameCharacter::SetRagdollOnBodyPart] Invalid body part enum sent to server"))) return;
	m_RagdollState[(int32)Part] = bNewRagdoll ? 1 : 0;
	OnRep_RagdollState();
}

bool AParkourGameCharacter::SetFullRagdoll_Validate(bool bIsFullRagdoll) { return true; }
void AParkourGameCharacter::SetFullRagdoll_Implementation(bool bIsFullRagdoll)
{
	m_RagdollState[(int32)EBodyPart::MAX] = bIsFullRagdoll ? 1 : 0;
	OnRep_RagdollState();
}

void AParkourGameCharacter::OnRep_RagdollState()
{
	USkeletalMeshComponent* PlayerMesh = GetSkeletalMesh();
	
	if (m_RagdollState[(int32)EBodyPart::MAX] > 0)
	{
		PlayerMesh->SetSimulatePhysics(true);
		return;
	}
	else
	{
		PlayerMesh->SetSimulatePhysics(false);
	}

	for (int32 i = 0; i < (int32)EBodyPart::MAX; ++i)
	{
		PlayerMesh->SetAllBodiesBelowSimulatePhysics(
			UParkourHelperLibrary::GetRootBoneForBodyPart((EBodyPart)i),
			m_RagdollState[i] > 0,
				true);
	}
}

void AParkourGameCharacter::CapsuleToRagdoll()
{
	USkeletalMeshComponent* PlayerMesh = GetSkeletalMesh();
	if (m_RagdollState[(int32)EBodyPart::MAX] > 0) {
		FVector SocketLocation = PlayerMesh->GetSocketLocation(UParkourHelperLibrary::GetRootBoneForBodyPart(EBodyPart::Pelvis));
		UCapsuleComponent* Capsule = GetCapsuleComponent();
		Capsule->SetWorldLocation(SocketLocation);
	}
}