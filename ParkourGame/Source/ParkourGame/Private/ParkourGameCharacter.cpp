#include "ParkourGameCharacter.h"

#include "ParkourMesh.h"
#include "Physics/ReverseSweep.h"
#include "ParkourInteractiveZone.h"
#include "MiniGame/MiniGameManager.h"
#include "Utils/ParkourFNames.h"
#include "Utils/SingletonHelper.h"
#include "Utils/ParkourHelperLibrary.h"
#include "Core/ParkourMovementComponent.h"
#include "Physics/ConstraintManager.h"
#include "Physics/SimpleSpringSystem.h"
#include "Physics/SpringSystem.h"
#include "Networking/ParkourPlayerState.h"
#include "Physics/PushSpringSystem.h"
#include "Audio/FootstepAudioTableRow.h"
#include "Spectator/ParkourSpectator.h"

// Engine
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Components/InputComponent.h"
#include "Components/TextRenderComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PlayerState.h"
#include "UnrealNetwork.h"
#include "PhysicsEngine/PhysicalAnimationComponent.h"
#include "Engine/DataTable.h"
#include "AudioDevice.h"
#include "PhysicalMaterials/PhysicalMaterial.h"

//////////////////////////////////////////////////////////////////////////
// AParkourGameCharacter

AParkourGameCharacter::AParkourGameCharacter(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer.SetDefaultSubobjectClass<UParkourMovementComponent>(ACharacter::CharacterMovementComponentName))
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
	PhysicalAnimation = CreateDefaultSubobject<UPhysicalAnimationComponent>(TEXT("PhysicalAnimation"));

	// Object detection sphere
	ObjectDetectionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("ObjectDetectionSphere"));
	ObjectDetectionSphere->SetSphereRadius(ObjectDetectionRadius);
	ObjectDetectionSphere->SetupAttachment(RootComponent);

	const float FootSphereRadius = 25.0f;

	FootSphereL = CreateDefaultSubobject<USphereComponent>(TEXT("FootSphereL"));
	FootSphereL->SetSphereRadius(FootSphereRadius);

	FootSphereR = CreateDefaultSubobject<USphereComponent>(TEXT("FootSphereR"));
	FootSphereR->SetSphereRadius(FootSphereRadius);
	
	USkeletalMeshComponent* SkelMesh = Cast<USkeletalMeshComponent>(GetComponentByClass(USkeletalMeshComponent::StaticClass()));

	if (SkelMesh)
	{
		FootSphereL->SetupAttachment(SkelMesh, FParkourFNames::Bone_Foot_L);
		FootSphereR->SetupAttachment(SkelMesh, FParkourFNames::Bone_Foot_R);
	}

	PlayerNameTag = CreateDefaultSubobject<UTextRenderComponent>(TEXT("PlayerName"));
	PlayerNameTag->SetupAttachment(RootComponent);

	SingletonHelper = MakeShareable(new FSingletonHelper);
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

	EnablePhysicalAnimation();

	OnActorBeginOverlap.AddDynamic(this, &AParkourGameCharacter::BeginOverlap);
	OnActorEndOverlap.AddDynamic(this, &AParkourGameCharacter::EndOverlap);

	if (PlayerNameTag)
	{
    if(PlayerNameTag->Text.IsEmpty())
		  PlayerNameTag->SetText(FText::FromString(""));

		if (Role == ENetRole::ROLE_AutonomousProxy)
		{
			PlayerNameTag->SetHiddenInGame(true);
		}
	}

	FootSphereL->OnComponentBeginOverlap.AddDynamic(this, &AParkourGameCharacter::PlayFootstepL);
	FootSphereR->OnComponentBeginOverlap.AddDynamic(this, &AParkourGameCharacter::PlayFootstepR);

	SetVisibleInXRay(true);

	//If there is an 'autojoin' game in progress, join it

	AMiniGameManager* Mgr = SingletonHelper->GetSingletonObject<AMiniGameManager>(GetWorld());
	if (Mgr) {
		AMiniGameBase* ActiveGame = Mgr->GetActiveGame();
		if (ActiveGame) {
			if (ActiveGame->AutoJoin) Mgr->AddPlayerToGame(this);
		}
	}
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
		if(!IsWithinFieldOfView(PMesh->GetActorLocation())) continue;

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

	MovementComp = Cast<UParkourMovementComponent>(GetCharacterMovement());
	SkeletalMesh = Cast<USkeletalMeshComponent>(GetComponentByClass(USkeletalMeshComponent::StaticClass()));

	LegSpring = NewObject<USimpleSpringSystem>(this);

	for (int32 i = 0; i < (int32)EHandSideEnum::MAX; ++i)
	{
		/*
		m_PushData[i].ArmSpring = NewObject<UPushSpringSystem>(this);
		m_PushData[i].ArmSpring->SpringConstant = 5000.0f;
		m_PushData[i].ArmSpring->SpringDampening = 100.0f;
		*/
		m_VaultData[i].ArmSpring = NewObject<UPushSpringSystem>(this);
		m_GripData[i].ArmSpring = NewObject<USpringSystem>(this);
		m_GripData[i].ArmSpring->SpringConstant = 5000.0f;
		m_GripData[i].ArmSpring->SpringDampening = 100.0f;
	}
}

void AParkourGameCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AParkourGameCharacter, m_RagdollState);
	DOREPLIFETIME(AParkourGameCharacter, m_GripData);
  DOREPLIFETIME(AParkourGameCharacter, StandUpAnimRow);
}

void AParkourGameCharacter::OnRep_PlayerState()
{
	if (PlayerState && PlayerState != ParkourPlayerState)
	{
		if (ParkourPlayerState)
		{
			ParkourPlayerState->OnPlayerNameChanged.RemoveDynamic(this, &AParkourGameCharacter::OnPlayerNameChanged);
		}

		ParkourPlayerState = Cast<AParkourPlayerState>(PlayerState);

		if (ParkourPlayerState)
		{
			ParkourPlayerState->OnPlayerNameChanged.AddDynamic(this, &AParkourGameCharacter::OnPlayerNameChanged);
			OnPlayerNameChanged();
		}
	}
}

void AParkourGameCharacter::OnPlayerNameChanged()
{
	if (PlayerNameTag)
		PlayerNameTag->SetText(FText::FromString(ParkourPlayerState->PlayerName));
}

void AParkourGameCharacter::PlayStandUpAnimation()
{
  if (!StandUpAnimationTable) return;
  
  FStandUpMontageRow* Row = StandUpAnimationTable->FindRow<FStandUpMontageRow>(StandUpAnimRow, "");
  
  if (!Row || !Row->Montage) return;

  PlayAnimMontage(Row->Montage);
 
  if(AController* Controller = GetController())
    Controller->SetIgnoreMoveInput(true);
  
  GetWorld()->GetTimerManager().ClearTimer(ResetStandupHandle);
  GetWorld()->GetTimerManager().SetTimer(ResetStandupHandle, FTimerDelegate::CreateUObject(this, &AParkourGameCharacter::ResetStandupAnim), FMath::Max(0.1f, Row->Montage->GetSectionLength(0) - 1.0f), false);
}

FName AParkourGameCharacter::ChooseStandUpAnimation(EStandUpDirection Direction) const
{
  if (!StandUpAnimationTable)
    return NAME_None;

  TArray<FName> RowNames = StandUpAnimationTable->GetRowNames();
  TArray<FStandUpMontageRow*> ValidRows;
  StandUpAnimationTable->GetAllRows<FStandUpMontageRow>("", ValidRows);

  for (int32 i = ValidRows.Num()-1; i >=0; --i)
  {
    if (ValidRows[i]->Direction != Direction)
    {
      ValidRows.RemoveAtSwap(i);
      RowNames.RemoveAtSwap(i);
    }
  }

  if (RowNames.Num() < 1) return NAME_None;
  return RowNames[FMath::RandHelper(RowNames.Num())];
}

void AParkourGameCharacter::ResetStandupAnim()
{
  if (AController* Controller = GetController())
    Controller->SetIgnoreMoveInput(false);

  if (HasAuthority())
  {
    // server should update to clients when the animation is over so that lagging clients don't play the animation stupidly late
    StandUpAnimRow = NAME_None;
  }
}

void AParkourGameCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	CapsuleToRagdoll();

	// tick the physics as often as is specified
	m_PhysicsClock += DeltaSeconds;
	const float PhysicsSubtickDeltaSeconds = 1.0f / (float)PhysicsSubstepTargetFramerate;

	while (m_PhysicsClock > PhysicsSubtickDeltaSeconds)
	{
		SubtickPhysics(PhysicsSubtickDeltaSeconds);
		m_PhysicsClock -= PhysicsSubtickDeltaSeconds;
	}

	//gripping code
	// due to the movement component only ticking at normal times we must only apply force everyt ime
	FVector TotalForce = FVector::ZeroVector;

	for (int32 i = 0; i < (int32)EHandSideEnum::MAX; ++i)
	{
		if (m_GripData[i].ArmSpring && m_GripData[i].isGripping) {
			m_GripData[i].ArmSpring->Point2 = GetSkeletalMesh()->GetBoneLocation(i == (int32)EHandSideEnum::HS_Left ? FParkourFNames::Bone_Upperarm_L : FParkourFNames::Bone_Upperarm_R, EBoneSpaces::WorldSpace);
			m_GripData[i].ArmSpring->Tick(DeltaSeconds);
			TotalForce += m_GripData[i].ArmSpring->GetSpringForce();
		} else if (m_VaultData[i].isVaulting) {
			if (FVector::DistSquared(m_VaultData[i].vaultTarget, GetSkeletalMesh()->GetBoneLocation(i == (int32)EHandSideEnum::HS_Left ? FParkourFNames::Bone_Upperarm_L : FParkourFNames::Bone_Upperarm_R, EBoneSpaces::WorldSpace)) > FMath::Square(100.0f))
				Server_EndGrip((EHandSideEnum)i);
		}
		/*else if (m_PushData[i].ArmSpring && m_PushData[i].isPushing) {
			m_PushData[i].ArmSpring->Point3 = GetSkeletalMesh()->GetBoneLocation(i == (int32)EHandSideEnum::HS_Left ? FParkourFNames::Bone_Upperarm_L : FParkourFNames::Bone_Upperarm_R, EBoneSpaces::WorldSpace);
			m_PushData[i].ArmSpring->Tick(DeltaSeconds);
		}*/
	}

	GetParkourMovementComp()->AddForce(TotalForce);

	if (PlayerNameTag)
		PlayerNameTag->SetWorldRotation(FQuat::Identity);
}


AParkourPlayerController* AParkourGameCharacter::GetParkourPlayerController() const
{
	return Cast<AParkourPlayerController>(Controller);
}

void AParkourGameCharacter::SubtickPhysics(float DeltaSeconds)
{
	BPE_SubtickPhysics(DeltaSeconds);

	// leg spring physics
	if(LegSpring)
		LegSpring->Tick(DeltaSeconds);
}

void AParkourGameCharacter::MoveForward(float Value)
{
	if (!Controller || Value == 0.0f || m_RagdollState[(int32)EBodyPart::MAX] > 0)
		return;

	// find out which way is forward
	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);
  
	// get forward vector
	const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	AddMovementInput(Direction, Value);
}

void AParkourGameCharacter::MoveRight(float Value)
{
	if (!Controller || Value == 0.0f || m_RagdollState[(int32)EBodyPart::MAX] > 0)
		return;
	
	// find out which way is right
	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);
	
	// get right vector 
	const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	// add movement in that direction
	AddMovementInput(Direction, Value);
}

void AParkourGameCharacter::Jump()
{
	if (m_RagdollState[(int32)EBodyPart::MAX] > 0)
		return;

	Super::Jump();
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

bool AParkourGameCharacter::GetParkourTarget(EHandSideEnum HandSide, FParkourTarget& Target)
{
  FVector HandPos;
  if (HandSide == EHandSideEnum::HS_Left)
  {
    HandPos = GetMesh()->GetSocketLocation("Hand_lSocket");
  }
  else
  {
    HandPos = GetMesh()->GetSocketLocation("Hand_rSocket");
  }

  return GetParkourTargetClosestTo(HandPos, Target);
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

void AParkourGameCharacter::BeginGrip(EHandSideEnum Hand)
{
	if (Hand == EHandSideEnum::MAX)
		return;

	Server_BeginGrip(Hand);
}

void AParkourGameCharacter::EndGrip(EHandSideEnum Hand)
{
	if (Hand == EHandSideEnum::MAX)
		return;

	Server_EndGrip(Hand);
}

void AParkourGameCharacter::GetVisualTargets(const FVector& Start, TArray<FHitResult>& outVisualHits) const
{
	TArray<TEnumAsByte<EObjectTypeQuery>> TraceObjectTypes;

	TraceObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic));

	FCollisionQueryParams TraceParams(FName(TEXT("Hand Trace")), true, this);

  const int32 numSteps = 12;
  outVisualHits.Empty(numSteps);

	for (int i = 0; i < numSteps; i++) {
    const float stepSize = 8.f;
		float r = i * stepSize;
		FHitResult Hit(ForceInit);
		const FVector Rot = GetActorRotation().Add(0.5f * numSteps * stepSize, 0.f, 0.f).Add(-r, 0.f, 0.f).Vector();
		const FVector End = Start + Rot * 2048;

		GetWorld()->LineTraceSingleByObjectType(
			Hit,
			Start,
			End,
			FCollisionObjectQueryParams(TraceObjectTypes)
		);
    
    //DrawDebugLine(GetWorld(), Start, End, FColor::Red);

		if (!Hit.GetActor())
      continue;
		
		bool rej = false;

		// Reject duplicate points on vertical surfaces
    for (const FHitResult& VHit : outVisualHits)
    {
      if (FVector::DistXY(VHit.ImpactPoint, Hit.ImpactPoint) < 10.f)
      {
        rej = true;
        break;
      }
    }

		// Reject points on horizontal surfaces
		if (FVector::Coincident(Hit.ImpactNormal, FVector(0.f, 0.f, -1.f)))
			rej = true;

		// Reject points on horizontal surfaces
		if (FVector::Coincident(Hit.ImpactNormal, FVector(0.f, 0.f, 1.f)))
			rej = true;

		if (rej == false) {
			outVisualHits.Add(Hit);
		}
	}
}

void AParkourGameCharacter::GetParkourTargets(TArray<FParkourTarget>& outPTargs, const TArray<FHitResult>& VHits) const
{
	FCollisionShape HandCol = FCollisionShape::MakeCapsule(5.f, 25.f);
	FRotator facing = GetControlRotation();

	facing.SetComponentForAxis(EAxis::Y, 0.f);

  outPTargs.Empty(VHits.Num());

	for (const FHitResult& VHit : VHits)
  {
		FRotator rot = FRotator(0.f, VHit.ImpactNormal.Rotation().Yaw, 90.f);
		FVector location = VHit.ImpactPoint;

		FVector gripTarget = location + GeomReverseSweep(
			GetWorld(), HandCol, FQuat(rot),
			location,
			location + FVector(0.f, 0.f, 1024.f),
			ECollisionChannel::ECC_GameTraceChannel1,
			FCollisionQueryParams::DefaultQueryParam,
			FCollisionResponseParams::DefaultResponseParam
		);

		FVector vaultEnd = gripTarget + GeomReverseSweep(
			GetWorld(), HandCol, FQuat(rot),
			gripTarget,
			gripTarget + (facing.Vector() * 1024),
			ECollisionChannel::ECC_GameTraceChannel1,
			FCollisionQueryParams::DefaultQueryParam,
			FCollisionResponseParams::DefaultResponseParam
		);

    FParkourTarget PTarg;
    PTarg.Target = location;
    PTarg.Rot = rot;
    PTarg.GripTarget = gripTarget;
    PTarg.VaultTarget = vaultEnd;

    outPTargs.Add(PTarg);
	}

}

bool AParkourGameCharacter::GetParkourTargetClosestTo(const FVector& Location, FParkourTarget& outTarget)
{
  TArray<FHitResult> VHits;
  TArray<FParkourTarget> PTargs;

  GetVisualTargets(Location, VHits);
  GetParkourTargets(PTargs, VHits);

  if (PTargs.Num() == 0) return false;

  float curDist = FLT_MAX;

  for (const FParkourTarget& PTarg : PTargs)
  {
    float dist = (PTarg.Target - Location).SizeSquared();
    if (dist < curDist)
    {
      curDist = dist;
      outTarget = PTarg;
    }
  }

  return true;
}

/*void AParkourGameCharacter::BeginPush(EHandSideEnum Hand)
{
	if (Hand == EHandSideEnum::MAX)
		return;

	UE_LOG(LogTemp, Warning, TEXT("Your message"));

	FPushData& Data = m_PushData[(int32)Hand];

	//location the PC is focused on
	const FVector Start = GetSkeletalMesh()->GetBoneLocation(Hand == EHandSideEnum::HS_Left ? FParkourFNames::Bone_Upperarm_L : FParkourFNames::Bone_Upperarm_R, EBoneSpaces::WorldSpace);

	FVector Rot = GetControlRotation().Vector();
	Rot.SetComponentForAxis(EAxis::Type::Z, 0.f);
	//256 units in facing direction of PC (256 units in front of the camera)
	const FVector End = Start + Rot * 1024;

	Data.isPushing = true;
	Data.ArmSpring->Initialise(Start, End, GetParkourPlayerController()->GetPawn());
	Data.pushTarget = Data.ArmSpring->Point1;
}

void AParkourGameCharacter::EndPush(EHandSideEnum Hand)
{
	if (Hand == EHandSideEnum::MAX) return;

	float force = m_PushData[(int32)Hand].ArmSpring->GetSpringForce();
	LaunchCharacter(GetControlRotation().Vector() * force, false, false);
	UE_LOG(LogTemp, Warning, TEXT("Your springforce is %s"), *FString::SanitizeFloat(force));
	
	m_PushData[(int32)Hand].isPushing = false;
}*/

bool AParkourGameCharacter::Server_BeginGrip_Validate(EHandSideEnum Hand) { return true; }

void AParkourGameCharacter::Server_BeginGrip_Implementation(EHandSideEnum Hand)
{
	if (Hand == EHandSideEnum::MAX)
		return;
	
	FParkourTarget Target;
	if (!GetParkourTarget(Hand, Target)) return;

	FVector pathToActor = Target.GripTarget - GetSkeletalMesh()->GetBoneLocation(FParkourFNames::Bone_Neck);
	float DistSqrd = pathToActor.SizeSquared();

	if (DistSqrd > FMath::Square(150.0f))
		return;

	if (pathToActor.Z > 0.f) {
		//GRIP
		FGripData& Data = m_GripData[(int32)Hand];

		Data.gripTarget = Target.GripTarget;
		Data.isGripping = true;
		OnRep_GripData();
	} else {
		//VAULT
		FVaultData& Data = m_VaultData[(int32)Hand];

		Data.vaultTarget = Target.GripTarget;
		Data.isVaulting = true;
		OnRep_VaultData();
	}
}

bool AParkourGameCharacter::Server_EndGrip_Validate(EHandSideEnum Hand) { return true; }

void AParkourGameCharacter::Server_EndGrip_Implementation(EHandSideEnum Hand)
{
	if (Hand == EHandSideEnum::MAX) return;

	m_GripData[(int32)Hand].isGripping = false;
	if (m_VaultData[(int32)Hand].isVaulting) {
		m_VaultData[(int32)Hand].isVaulting = false;
		FVector Shoulder = GetSkeletalMesh()->GetBoneLocation(Hand == EHandSideEnum::HS_Left ? FParkourFNames::Bone_Upperarm_L : FParkourFNames::Bone_Upperarm_R, EBoneSpaces::WorldSpace);
		LaunchCharacter(m_VaultData[(int32)Hand].ArmSpring->GetSpringForce(Shoulder), false, false);
	}
}

void AParkourGameCharacter::StandUp()
{
	if (!IsFullRagdoll() || GetSkeletalMesh()->GetComponentVelocity().Z < -5.0f) return;

	SetFullRagdoll(false);
}

//////////////////////////////////////////////////////////////////////////
// Input

void AParkourGameCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	// helper macro that allows us to pass payload arguments through to input functions
#define BIND_ACTION_CUSTOMEVENT(ActionName, KeyEvent, Func, ...) \
	{ \
		FInputActionBinding AB(ActionName, KeyEvent); \
		AB.ActionDelegate = FInputActionUnifiedDelegate(FInputActionHandlerSignature::CreateUObject(this, Func, ##__VA_ARGS__)); \
		PlayerInputComponent->AddActionBinding(AB); \
	}

	// Set up gameplay key bindings
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AParkourGameCharacter::Jump);
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
	PlayerInputComponent->BindAction("StandUp", IE_Pressed, this, &AParkourGameCharacter::StandUp);

	// TEMP DISABLED -- for physical animation
	PlayerInputComponent->BindAction("RagdollArmR", IE_Pressed, this, &AParkourGameCharacter::RagdollArmR);
	PlayerInputComponent->BindAction("RagdollArmL", IE_Pressed, this, &AParkourGameCharacter::RagdollArmL);
	PlayerInputComponent->BindAction("RagdollLegR", IE_Pressed, this, &AParkourGameCharacter::RagdollLegR);
	PlayerInputComponent->BindAction("RagdollLegL", IE_Pressed, this, &AParkourGameCharacter::RagdollLegL);
	PlayerInputComponent->BindAction("RagdollTorso", IE_Pressed, this, &AParkourGameCharacter::RagdollTorso);
	PlayerInputComponent->BindAction("RagdollLegs", IE_Pressed, this, &AParkourGameCharacter::RagdollLegs);

	//bindings for minigames
	PlayerInputComponent->BindAction(FParkourFNames::Input_JoinGame, IE_Pressed, this, &AParkourGameCharacter::JoinMinigame);

	// grip controls
	BIND_ACTION_CUSTOMEVENT("GripL", IE_Pressed, &AParkourGameCharacter::BeginGrip, EHandSideEnum::HS_Left);
	BIND_ACTION_CUSTOMEVENT("GripL", IE_Released, &AParkourGameCharacter::EndGrip, EHandSideEnum::HS_Left);
	BIND_ACTION_CUSTOMEVENT("GripR", IE_Pressed, &AParkourGameCharacter::BeginGrip, EHandSideEnum::HS_Right);
	BIND_ACTION_CUSTOMEVENT("GripR", IE_Released, &AParkourGameCharacter::EndGrip, EHandSideEnum::HS_Right);
	/*BIND_ACTION_CUSTOMEVENT("PushR", IE_Pressed, &AParkourGameCharacter::BeginPush, EHandSideEnum::HS_Right);
	BIND_ACTION_CUSTOMEVENT("PushR", IE_Released, &AParkourGameCharacter::EndPush, EHandSideEnum::HS_Right);
	BIND_ACTION_CUSTOMEVENT("PushL", IE_Pressed, &AParkourGameCharacter::BeginPush, EHandSideEnum::HS_Left);
	BIND_ACTION_CUSTOMEVENT("PushL", IE_Released, &AParkourGameCharacter::EndPush, EHandSideEnum::HS_Left);*/

#undef BIND_ACTION_CUSTOMEVENT
}

void AParkourGameCharacter::PlayFootstepL(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherComp != GetCharacterMovement()->GetMovementBase()) return;

	int32 SoundType = 0;

	if (OtherComp)
	{
		SoundType = (int32)OtherComp->BodyInstance.GetSimplePhysicalMaterial()->SurfaceType - 1;
	}

	PlayFootstepSound(SoundType, true);
}

void AParkourGameCharacter::PlayFootstepR(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherComp != GetCharacterMovement()->GetMovementBase()) return;

	int32 SoundType = 0;

	if (OtherComp)
	{
		SoundType = (int32)OtherComp->BodyInstance.GetSimplePhysicalMaterial()->SurfaceType - 1;
	}

	PlayFootstepSound(SoundType, false);
}

void AParkourGameCharacter::PlayFootstepSound(int32 SoundType, bool isLeft)
{
	UWorld* WorldPtr = GetWorld();

	if (!WorldPtr || !FootstepAudioTable)
		return;
	
	TArray<FName> RowNames = FootstepAudioTable->GetRowNames();

	if (RowNames.Num() == 0) return;

	if (!RowNames.IsValidIndex(SoundType))
		SoundType = 0;

	const FFootstepAudioTableRow* RowPtr = FootstepAudioTable->FindRow<FFootstepAudioTableRow>(RowNames[SoundType], "PlayFootstepSound", false);

	if (!RowPtr) return;

	// select a sound randomly
	USoundWave* FoundSound = nullptr;

	if (isLeft)
		FoundSound = RowPtr->LeftFoot[FMath::RandHelper(RowPtr->LeftFoot.Num() - 1)];
	else
		FoundSound = RowPtr->RightFoot[FMath::RandHelper(RowPtr->LeftFoot.Num() - 1)];

	if (!FoundSound) return;

	if (FAudioDevice* AudioDevice = WorldPtr->GetAudioDevice())
	{
		FVector Location;
		AudioDevice->PlaySoundAtLocation(FoundSound, WorldPtr, 1.0f, 1.0f, 0.0f, Location, FRotator::ZeroRotator, nullptr, nullptr, nullptr, this);
	}
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
	OnRagdoll.Broadcast(this);
}

bool AParkourGameCharacter::IsFullRagdoll() const
{
	return m_RagdollState[(int32)EBodyPart::MAX] > 0;
}

// Should only be used in editor - switching to a spectator mid game could break minigames in a bad way
#if WITH_EDITOR

static void cmd_BecomeSpectator(UWorld* World)
{
	APlayerController* PlayerCtlr = World->GetFirstPlayerController();
	AParkourGameCharacter* PlayerCharacter = PlayerCtlr ? Cast<AParkourGameCharacter>(PlayerCtlr->GetPawn()) : nullptr;

	if (!IsValid(PlayerCharacter))
		return;

	PlayerCharacter->Server_BecomeSpectator();
}

FAutoConsoleCommandWithWorld BecomeSpectatorCmd(
	TEXT("Parkour.Spectate"),
	TEXT("Changes the player into a spectator"),
	FConsoleCommandWithWorldDelegate::CreateStatic(cmd_BecomeSpectator));

#endif


bool AParkourGameCharacter::Server_BecomeSpectator_Validate()
{
	return true;
}

void AParkourGameCharacter::Server_BecomeSpectator_Implementation()
{
#if WITH_EDITOR
	AParkourSpectator* SpecatorPawn = GetWorld()->SpawnActor<AParkourSpectator>(AParkourSpectator::StaticClass(), GetTransform());

	GetController()->Possess(SpecatorPawn);
	Destroy();
#endif
}

void AParkourGameCharacter::GetGripData(EHandSideEnum Hand, FGripData& Data) const
{
	if (Hand == EHandSideEnum::MAX) return;
	Data = m_GripData[(int32)Hand];
}

void AParkourGameCharacter::GetVaultData(EHandSideEnum Hand, FVaultData& Data) const
{
	if (Hand == EHandSideEnum::MAX) return;
	Data = m_VaultData[(int32)Hand];
}
void AParkourGameCharacter::SetVisibleInXRay(bool ShouldBeVisible)
{
	if (Role == ENetRole::ROLE_AutonomousProxy) return;

	GetSkeletalMesh()->SetRenderCustomDepth(ShouldBeVisible);
	GetSkeletalMesh()->SetCustomDepthStencilValue(ShouldBeVisible ? 255 : 0);
}

/*
void AParkourGameCharacter::GetPushData(EHandSideEnum Hand, FPushData& Data) const
{
	if (Hand == EHandSideEnum::MAX) return;
	Data = m_PushData[(int32)Hand];
}*/

bool AParkourGameCharacter::IsWithinFieldOfView(const FVector& Location) const
{
	FVector Dir = Location - GetActorLocation();
	Dir.Normalize();

	const float Angle = FMath::Acos(FVector::DotProduct(Dir, GetActorForwardVector()));
	return FMath::Abs(Angle) < 1.57f;
}

void AParkourGameCharacter::EnablePhysicalAnimation(bool Enable /*= true*/)
{
	if (Enable)
	{
		PhysicalAnimation->SetSkeletalMeshComponent(GetSkeletalMesh());
		PhysicalAnimation->ApplyPhysicalAnimationProfileBelow(FParkourFNames::Bone_Pelvis, FParkourFNames::Profile_Fall, false);
	}
	else
	{
		PhysicalAnimation->SetSkeletalMeshComponent(nullptr);
	}
} 

void AParkourGameCharacter::OnRep_RagdollState()
{
	USkeletalMeshComponent* PlayerMesh = GetSkeletalMesh();
	
	if (m_RagdollState[(int32)EBodyPart::MAX] > 0)
	{
		EnablePhysicalAnimation(false);
		PlayerMesh->ResetAllBodiesSimulatePhysics();
		GetParkourMovementComp()->SetMovementMode(MOVE_None);
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		PlayerMesh->SetSimulatePhysics(true);
		return;
	}
	else
	{
		UCapsuleComponent* Capsule = GetCapsuleComponent();

    // calculate the facing of the ragdoll and the direction so the standing capsule can match
    const FVector HeadLocation = PlayerMesh->GetBoneLocation(FParkourFNames::Bone_Head);
    const FVector FootLocation = (PlayerMesh->GetBoneLocation(FParkourFNames::Bone_Foot_L) + PlayerMesh->GetBoneLocation(FParkourFNames::Bone_Foot_R)) * 0.5f;
    const FVector NeckForward = PlayerMesh->GetBoneQuaternion(FParkourFNames::Bone_Neck).GetForwardVector();

    const EStandUpDirection StandUpDir = NeckForward.Z > 0.0f ? EStandUpDirection::FaceUp : EStandUpDirection::FaceDown;

    FVector RagdollDirection = HeadLocation - FootLocation;
    RagdollDirection.Normalize();
    RagdollDirection.Z = 0.0f;

    // if the player is face up we then the head is close to the camera so we need to reverse
    if (StandUpDir == EStandUpDirection::FaceUp) RagdollDirection *= -1;

    SetActorRotation(FRotationMatrix::MakeFromX(RagdollDirection).Rotator());
    
		PlayerMesh->SetAllBodiesBelowSimulatePhysics(UParkourHelperLibrary::GetRootBoneForBodyPart(EBodyPart::Pelvis), false, true);
    PlayerMesh->AttachToComponent(Capsule, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, true));
		PlayerMesh->SetRelativeLocationAndRotation(FVector(0.0, 0.0, -90.0), FRotator(0.0, 270.0, 0.0), false, (FHitResult *)nullptr, ETeleportType::None);
		GetParkourMovementComp()->SetMovementMode(MOVE_Walking);
    Capsule->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		EnablePhysicalAnimation();

    if (HasAuthority())
    {
      StandUpAnimRow = ChooseStandUpAnimation(StandUpDir);
      OnRep_StandUpAnimRow();
    }
	}
	for (int32 i = 0; i < (int32)EBodyPart::MAX; ++i)
	{
		PlayerMesh->SetAllBodiesBelowSimulatePhysics(
			UParkourHelperLibrary::GetRootBoneForBodyPart((EBodyPart)i),
			m_RagdollState[i] > 0,
				true);
	}
}

void AParkourGameCharacter::OnRep_GripData()
{
	for (int32 i = 0; i < (int32)EHandSideEnum::MAX; ++i)
	{
		if (m_GripData[i].isGripping)
		{
			if (m_GripData[i].ArmSpring->GetPoint1Location() != m_GripData[i].gripTarget)
				m_GripData[i].ArmSpring->Initialise(m_GripData[i].gripTarget, m_GripData[i].gripTarget);
		}
		else
		{
			m_GripData[i].ArmSpring->Initialise(FVector::ZeroVector, FVector::ZeroVector);
		}
	}
}

void AParkourGameCharacter::OnRep_VaultData()
{
	for (int32 i = 0; i < (int32)EHandSideEnum::MAX; ++i)
	{
		FVector Shoulder = GetSkeletalMesh()->GetBoneLocation(i == (int32)EHandSideEnum::HS_Left ? FParkourFNames::Bone_Upperarm_L : FParkourFNames::Bone_Upperarm_R, EBoneSpaces::WorldSpace);
		if (m_VaultData[i].isVaulting)
		{
			if (m_VaultData[i].ArmSpring->GetPoint1Location() != m_VaultData[i].vaultTarget)
				m_VaultData[i].ArmSpring->Initialise(m_VaultData[i].vaultTarget, Shoulder);
		}
		else
		{

			m_VaultData[i].ArmSpring->Initialise(FVector::ZeroVector, FVector::ZeroVector);
		}
	}
}

void AParkourGameCharacter::OnRep_StandUpAnimRow()
{
  if (StandUpAnimRow == NAME_None) return;

  PlayStandUpAnimation();
}

void AParkourGameCharacter::CapsuleToRagdoll()
{
	USkeletalMeshComponent* PlayerMesh = GetSkeletalMesh();
	if (m_RagdollState[(int32)EBodyPart::MAX] > 0) {
		FVector SocketLocation = PlayerMesh->GetSocketLocation(UParkourHelperLibrary::GetRootBoneForBodyPart(EBodyPart::Pelvis));
		UCapsuleComponent* Capsule = GetCapsuleComponent();
		Capsule->SetWorldLocation(SocketLocation + FVector(0.0, 0.0, 97.0));
	}
}

bool AParkourGameCharacter::Server_JoinMinigame_Validate() { return true; }
void AParkourGameCharacter::Server_JoinMinigame_Implementation()
{
	AMiniGameManager* Mgr = SingletonHelper->GetSingletonObject<AMiniGameManager>(GetWorld());
	if (!Mgr) return;

	Mgr->AddPlayerToGame(this);
}