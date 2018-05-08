#include "ParkourGameCharacter.h"

#include "ParkourMesh.h"
#include "Physics/ReverseSweep.h"
#include "ParkourInteractiveZone.h"
#include "MiniGame/MiniGameManager.h"
#include "MiniGame/BallBase.h"
#include "Utils/ParkourFNames.h"
#include "Utils/SingletonHelper.h"
#include "Utils/ParkourHelperLibrary.h"
#include "Core/ParkourMovementComponent.h"
#include "Physics/ConstraintManager.h"
#include "Physics/SimpleSpringSystem.h"
#include "Physics/SpringSystem.h"
#include "Physics/PushSpringSystem.h"
#include "Networking/ParkourPlayerState.h"
#include "Audio/FootstepAudioTableRow.h"
#include "Spectator/ParkourSpectator.h"
#include "UI/ParkourGameHUD.h"

// Engine
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Components/InputComponent.h"
#include "Components/TextRenderComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/GameStateBase.h"
#include "UnrealNetwork.h"
#include "PhysicsEngine/PhysicalAnimationComponent.h"
#include "Engine/DataTable.h"
#include "AudioDevice.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "EngineUtils.h"
#include "AI/Navigation/NavigationSystem.h"
#include "Runtime/Engine/Classes/Kismet/KismetSystemLibrary.h"
#include "TimerManager.h"

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

  Hat = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Hat"));
  Hat->SetupAttachment(SkelMesh, FParkourFNames::Bone_Head);
  Hat->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	SingletonHelper = MakeShareable(new FSingletonHelper);
  
#if !WITH_EDITOR
  // disable screen messages by default in all non-editor builds
  GAreScreenMessagesEnabled = false;
#endif
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

#if WITH_EDITOR
  // DEBUG OPTIONS FOR SHOWING THE CAPSULE
  // Set to false to view the capsule at all times
	GetCapsuleComponent()->SetHiddenInGame(true);
#else
  // DO NOT TOUCH THIS, modify the code above instead
  GetCapsuleComponent()->SetHiddenInGame(true);
#endif

  GetCapsuleComponent()->GetUnscaledCapsuleSize(DefaultCapsuleRadius, DefaultCapsuleHalfHeight);

	EnablePhysicalAnimation();
  ResetAFKTimer();

	OnActorBeginOverlap.AddDynamic(this, &AParkourGameCharacter::BeginOverlap);
	OnActorEndOverlap.AddDynamic(this, &AParkourGameCharacter::EndOverlap);

  OnRagdoll.AddDynamic(this, &AParkourGameCharacter::OnRagdollEvent);

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

  AMiniGameManager* GameManager = SingletonHelper->GetSingletonObject<AMiniGameManager>(GetWorld());

  if (GameManager)
  {
    GameManager->OnPlayerJoinedTeam.AddDynamic(this, &AParkourGameCharacter::OnJoinedTeam);
    GameManager->OnGameOver.AddDynamic(this, &AParkourGameCharacter::OnGameOver);
  }

  APlayerController* ControllerPtr = Cast<APlayerController>(GetController());
  AParkourGameHUD* HUDptr = Cast<AParkourGameHUD>(ControllerPtr ? ControllerPtr->GetHUD() : nullptr);
  if (HUDptr)
    HUDptr->InitialisePlayerUI();
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
		m_GripData[i].ArmSpring->SpringDampening = 110.0f;
	}
}

void AParkourGameCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AParkourGameCharacter, m_RagdollState);
	DOREPLIFETIME(AParkourGameCharacter, m_GripData);
  DOREPLIFETIME(AParkourGameCharacter, isVaulting);
  DOREPLIFETIME(AParkourGameCharacter, isRolling);
  DOREPLIFETIME(AParkourGameCharacter, isFlipping);
}

FVector AParkourGameCharacter::GetVelocity() const
{
  if (IsFullRagdoll()) return GetSkeletalMesh()->GetComponentVelocity();
  return Super::GetVelocity();
}

void AParkourGameCharacter::FellOutOfWorld(const class UDamageType& dmgType)
{
  if (HasAuthority())
  {
    AController* OurController = GetController();

    Super::FellOutOfWorld(dmgType);

    GetWorld()->GetAuthGameMode()->RestartPlayer(OurController);
  }
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

void AParkourGameCharacter::PlayStandUpAnimation(const FName& StandUpAnimRow)
{
  if (!StandUpAnimationTable) return;
  
  FStandUpMontageRow* Row = StandUpAnimationTable->FindRow<FStandUpMontageRow>(StandUpAnimRow, "");
  
  if (!Row || !Row->Montage) return;

  const float PlayRate = 2.0f;
  PlayAnimMontage(Row->Montage, PlayRate);
 
  if(AController* ControllerPtr = GetController())
    ControllerPtr->SetIgnoreMoveInput(true);

  EnableJumping(false);
  EnableRolling(false);

  GetWorld()->GetTimerManager().ClearTimer(ResetStandupHandle);
  GetWorld()->GetTimerManager().SetTimer(ResetStandupHandle, FTimerDelegate::CreateUObject(this, &AParkourGameCharacter::ResetStandupAnim), FMath::Max(0.1f, (Row->Montage->GetSectionLength(0) / (Row->Montage->RateScale * PlayRate)) - (0.5f * Row->Montage->BlendOut.GetBlendTime())), false);
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
  if (AController* ControllerPtr = GetController())
    ControllerPtr->SetIgnoreMoveInput(false);

  EnableJumping(true);
  EnableRolling(true);
}

FPlayerKeyframe AParkourGameCharacter::BP_CreateKeyframe_Implementation(const FPlayerKeyframe& Keyframe)
{
  return Keyframe;
}

bool AParkourGameCharacter::BP_ReplayKeyframe_Implementation(const FPlayerKeyframe& Keyframe)
{
  return true;
}

void AParkourGameCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	CapsuleToRagdoll();

	if (isRolling) 
  {
    // if the player does not press input add an artificial one
		if (MoveForward_mag == 0.0f && MoveRight_mag == 0.0f)
			MoveForward(1.0f);

		Tick_Roll(DeltaSeconds);

    if (HasAuthority())
    {
      StopRollingDelay = StopRollingDelay - DeltaSeconds;

      if (StopRollingDelay <= 0.0f)
      {
        isRolling = false;
        OnRep_IsRolling();
      }
    }
	}
	
  // OWNING CLIENT ONLY
  if (Role == ROLE_AutonomousProxy && IsFullRagdoll() && GetVelocity().SizeSquared() < 100.0f)
  {
    AutoStandUpTime -= DeltaSeconds;
    if (AutoStandUpTime < 0.0f)
    {
      StandUp();
      AutoStandUpTime = 0.2f;
    }
	}
  else
  {
    AutoStandUpTime = GetUpDelay;
  }

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
	MoveForward_mag = Value;
  if (!Controller || Value == 0.0f)
    return;

  ResetAFKTimer();
  if (m_RagdollState[(int32)EBodyPart::MAX] > 0) return;

	// find out which way is forward
	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);
  
	// get forward vector
	const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	AddMovementInput(Direction, Value);
}

void AParkourGameCharacter::MoveRight(float Value)
{
	MoveRight_mag = Value;
	if (!Controller || Value == 0.0f)
		return;

  ResetAFKTimer();

  if (m_RagdollState[(int32)EBodyPart::MAX] > 0) return;
	
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
	ResetAFKTimer();

	if (m_RagdollState[(int32)EBodyPart::MAX] > 0 || !bCanJump)
		return;

	if (m_GripData[(int32)EHandSideEnum::HS_Left].isGripping) { 
		Server_Vault(EHandSideEnum::HS_Left);
		return;
	}
	else if (m_GripData[(int32)EHandSideEnum::HS_Right].isGripping) {
		Server_Vault(EHandSideEnum::HS_Right);
		return;
	}

	Super::Jump();
}


void AParkourGameCharacter::Roll_Start() 
{
  // TODO: Remove
}


void AParkourGameCharacter::BeginRoll()
{
  if (isFlipping || isRolling || !bRollingEnabled) return;
  Server_BeginRoll();
}

bool AParkourGameCharacter::Server_BeginRoll_Validate()
{
  return true;
}

void AParkourGameCharacter::Server_BeginRoll_Implementation()
{
  if (isFlipping || isRolling || !bRollingEnabled) return;

  if (GetMovementComponent()->IsFalling())
  {
    isFlipping = true;
    OnRep_IsFlipping();
  }
  else if(!IsFullRagdoll() || bCanRollRecover)
  {
    isRolling = true;
    StopRollingDelay = RollDuration;

    SetFullRagdoll(false);

    OnRep_IsRolling();
  }
}

void AParkourGameCharacter::StartRollRecoverTimer()
{
  if (!HasAuthority()) return;

  bCanRollRecover = true;

  FTimerManager& Mgr = GetWorld()->GetTimerManager();
  Mgr.ClearTimer(RollRecoverHandle);
  Mgr.SetTimer(RollRecoverHandle, FTimerDelegate::CreateUObject(this, &AParkourGameCharacter::RollRecoverWindowEnded), RollRecoverWindow, false);
}

void AParkourGameCharacter::OnRep_IsRolling_Implementation()
{
  // initialise some variables
  UCapsuleComponent* capsule = GetCapsuleComponent();
  float OutRadius;
  float OutHalfHeight;
  capsule->GetUnscaledCapsuleSize(OutRadius, OutHalfHeight);

  // and this is the actual roll, called from the animation blueprint after the initial animation is finished, if called again while
  // rolling will exit roll
  if (isRolling) 
  {	  
    //become a ball!
    capsule->SetCapsuleSize(OutRadius * 2, OutRadius * 2, true);

    if (Role == ROLE_SimulatedProxy)
    {
      BaseTranslationOffset = FVector(0, 0, 0.0f);
    }
    else
    {
      GetSkeletalMesh()->SetRelativeLocation(FVector(0, 0, 0.0f));
    }

    EnableJumping(false);
  }
  else
  {
    capsule->SetCapsuleSize(DefaultCapsuleRadius, DefaultCapsuleHalfHeight, true);

    if (USkeletalMeshComponent* mesh = GetSkeletalMesh())
    {
      const FVector MeshOffset(0.0f, 0.0f, -87.0f);
      const FRotator MeshRotation(0.0f, -90.0f, 0.0f);

      if (Role == ROLE_SimulatedProxy)
      {
        BaseTranslationOffset = MeshOffset;
        BaseRotationOffset = MeshRotation.Quaternion();
      }
      else 
      {
        mesh->SetRelativeLocation(MeshOffset);
        mesh->SetRelativeRotation(MeshRotation);
      }

    }

    EnableJumping(true);
  }
}

// use this function to calculate how fast the ball should rotate
void AParkourGameCharacter::Tick_Roll(float DeltaSeconds)
{
	float OutRadius;
	float OutHalfHeight;
  GetCapsuleComponent()->GetUnscaledCapsuleSize(OutRadius, OutHalfHeight);

  const float speed = GetVelocity().Size2D();
	const float delta_rotation = speed * DeltaSeconds * 360 / (2 * 3.14* OutRadius);
  const FRotator Rotation = FRotator(0.0f, 0.0f, delta_rotation);

  if (Role == ROLE_SimulatedProxy)
  {
    BaseRotationOffset *= Rotation.Quaternion();
  }
  else
  {
    GetSkeletalMesh()->AddRelativeRotation(FRotator(0.0f, 0.0f, delta_rotation), false, nullptr, ETeleportType::None);
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

bool AParkourGameCharacter::GetParkourTarget(EHandSideEnum HandSide, FParkourTarget& Target)
{
  FVector Offset = GetActorRightVector() * 30.0f;
  
  if (HandSide == EHandSideEnum::HS_Left)
  {
    Offset *= -1;
  }

  Offset += GetActorForwardVector() * -20.0f;

  return GetParkourTargetClosestTo(GetActorLocation() + Offset, Target);
}

void AParkourGameCharacter::TurnAtRate(float Rate)
{
  if(!FMath::IsNearlyZero(Rate))
    ResetAFKTimer();

	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AParkourGameCharacter::LookUpAtRate(float Rate)
{
  if (!FMath::IsNearlyZero(Rate))
    ResetAFKTimer();

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
  ResetAFKTimer();
	if (Hand == EHandSideEnum::MAX)
		return;

	Server_BeginGrip(Hand);
}

void AParkourGameCharacter::EndGrip(EHandSideEnum Hand)
{
  ResetAFKTimer();
	if (Hand == EHandSideEnum::MAX)
		return;

	Server_EndGrip(Hand);
}

void AParkourGameCharacter::GetVisualTargets(const FVector& Start, TArray<FHitResult>& outVisualHits) const
{
	FCollisionQueryParams TraceParams(FName(TEXT("Hand Trace")), true, this);

  const int32 numSteps = 24;
  outVisualHits.Empty(numSteps);

	for (int i = 0; i < numSteps; i++)
  {
    const float stepSize = 8.0f;
		float r = i * stepSize;
		FHitResult Hit(ForceInit);
		const FVector Rot = GetActorRotation().Add(0.5f * numSteps * stepSize, 0.f, 0.f).Add(-r, 0.f, 0.f).Vector();
		const FVector End = Start + Rot * 2048;

		GetWorld()->LineTraceSingleByChannel(
			Hit,
			Start,
			End,
			ECollisionChannel::ECC_WorldStatic,
			TraceParams
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

		/*DrawDebugCapsule(
			GetWorld(),
			vaultEnd,
			25.f,
			5.f,
			FQuat(rot),
			FColor(255, 0, 0),
			true,
			4.f
		);*/

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

	//UE_LOG(LogTemp, Warning, TEXT("Your message"));

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

bool AParkourGameCharacter::Server_Vault_Validate(EHandSideEnum Hand) { return true; }

void AParkourGameCharacter::Server_Vault_Implementation(EHandSideEnum Hand)
{
	if (Hand == EHandSideEnum::MAX)
		return;

	//GetCharacterMovement()->Velocity = (facing.Vector() * 100);
	Server_EndGrip(EHandSideEnum::HS_Left);
	Server_EndGrip(EHandSideEnum::HS_Right);
	UpdateVault(Hand);
}

void AParkourGameCharacter::EndVaultAnim() {
	isVaulting = false;
}

bool AParkourGameCharacter::IsAutonomousProxy() const
{
  return Role == ROLE_AutonomousProxy;
}

void AParkourGameCharacter::UpdateVault(EHandSideEnum Hand) {
	FGripData& Data = m_GripData[(int32)Hand];

	FVector pathToActor = Data.gripTarget - GetSkeletalMesh()->GetBoneLocation(Hand == EHandSideEnum::HS_Left ? FParkourFNames::Bone_Hand_L : FParkourFNames::Bone_Hand_R);
	float DistSqrd = 2.5 * pathToActor.Size();
	FRotator facing = GetControlRotation();
  isVaulting = true;
	facing.SetComponentForAxis(EAxis::Y, 0.f);
	FLatentActionInfo info;
	info.CallbackTarget = this;
	info.ExecutionFunction = "EndVaultAnim";
	info.UUID = 1;
	info.Linkage = 0;
	UKismetSystemLibrary::MoveComponentTo(RootComponent, RootComponent->GetComponentLocation() + (DistSqrd * FVector::UpVector) + (facing.Vector() * 50), FRotator(0.0f, 0.0f, 0.0f), false, false, 0.5f, false, EMoveComponentAction::Type::Move, info);
}

bool AParkourGameCharacter::Server_BeginGrip_Validate(EHandSideEnum Hand) { return true; }

void AParkourGameCharacter::Server_BeginGrip_Implementation(EHandSideEnum Hand)
{
	if (Hand == EHandSideEnum::MAX)
		return;

  // if we grab a ball then we are done
  if (Server_GrabNearbyBall(Hand))
    return;
  else if (Server_DropBall(Hand))
    return;
	
	FParkourTarget Target;
	if (!GetParkourTarget(Hand, Target)) return;

	FVector pathToActor = Target.GripTarget - GetSkeletalMesh()->GetBoneLocation(Hand == EHandSideEnum::HS_Left ? FParkourFNames::Bone_Hand_L : FParkourFNames::Bone_Hand_R);
	float DistSqrd = pathToActor.SizeSquared();

	if (DistSqrd > FMath::Square(200.0f))
		return;

	if (pathToActor.Z > 0.f) {
		//GRIP
		FGripData& Data = m_GripData[(int32)Hand];

		Data.gripTarget = Target.GripTarget;
		Data.isGripping = true;
		OnRep_GripData();
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

bool AParkourGameCharacter::Server_GrabNearbyBall(EHandSideEnum Hand)
{
  if (!HasAuthority()) return false;

  if (m_GripData[(int32)Hand].HeldBall.IsValid()) return false;

  ABallBase* ClosestBall = nullptr;
  float ClosestBallDist_sqrd = 0.0f;

  FVector Position = GetActorLocation();

  for (TActorIterator<ABallBase> It(GetWorld(), BallClass); It; ++It)
  {
    float Dist_sqrd = ((*It)->GetActorLocation() - Position).SizeSquared();

    if((*It)->GetHeldBy() != nullptr)
      continue;

    if (!ClosestBall || Dist_sqrd < ClosestBallDist_sqrd)
    {
      ClosestBallDist_sqrd = Dist_sqrd;
      ClosestBall = *It;
    }
  }

  if (!ClosestBall) return false;
  if (ClosestBallDist_sqrd > FMath::Square(BallPickupDistance)) return false;

  m_GripData[(int32)Hand].HeldBall = ClosestBall;
  ClosestBall->SetHeldBy(this);

  OnRep_GripData();
  return true;
}

bool AParkourGameCharacter::Server_DropBall(EHandSideEnum Hand)
{
  if (!HasAuthority()) return false;

  ABallBase* HeldBallptr = m_GripData[(int32)Hand].HeldBall.Get();
  if (!HeldBallptr) return false;

  m_GripData[(int32)Hand].HeldBall.Reset();
  HeldBallptr->SetHeldBy(nullptr);
  OnRep_GripData();
  return true;
}

bool AParkourGameCharacter::HasBall() const
{
  for (int32 i = 0; i < (int32)EHandSideEnum::MAX; ++i)
  {
    if (m_GripData[i].HeldBall.IsValid()) return true;
  }

  return false;
}

void AParkourGameCharacter::OnRagdollEvent()
{
  if (!HasAuthority()) return;

  Server_DropBall(EHandSideEnum::HS_Left);
  Server_DropBall(EHandSideEnum::HS_Right);
}

bool AParkourGameCharacter::CreateKeyframe(FPlayerKeyframe& Keyframe)
{
  Keyframe.Velocity = GetVelocity();
  Keyframe.IsFullRagdoll = m_RagdollState[(int32)EBodyPart::MAX] > 0;
  Keyframe.IsInAir = !GetCharacterMovement()->IsMovingOnGround();
  Keyframe = BP_CreateKeyframe(Keyframe);
  return true;
}

bool AParkourGameCharacter::ReplayKeyframe(const FPlayerKeyframe& Keyframe)
{
  if (Keyframe.IsFullRagdoll != m_RagdollState[(int32)EBodyPart::MAX])
  {
    m_RagdollState[(int32)EBodyPart::MAX] = Keyframe.IsFullRagdoll ? 1 : 0;
    OnRep_RagdollState();
  }

  Replay_Velocity = Keyframe.Velocity;
  Replay_IsInAir = Keyframe.IsInAir;

  return BP_ReplayKeyframe(Keyframe);
}

void AParkourGameCharacter::InitialiseReplayActor_Implementation(AParkourGameCharacter* ReplayActor)
{
  if (!ReplayActor) return;

  // copy team materials and name
  ReplayActor->GetSkeletalMesh()->SetMaterial(0, GetSkeletalMesh()->GetMaterial(0));
  ReplayActor->GetHat()->SetMaterial(0, GetHat()->GetMaterial(0));
  ReplayActor->GetPlayerNameTag()->SetText(FText::FromString(ParkourPlayerState->PlayerName));
}

void AParkourGameCharacter::ResetAFKTimer()
{
#if !WITH_EDITOR
  if (Role != ROLE_AutonomousProxy) return;

  FTimerManager& Mgr = GetWorld()->GetTimerManager();
  
  Mgr.ClearTimer(AFKTimerHandle);
  AFKTimerHandle.Invalidate();

  Mgr.SetTimer(AFKTimerHandle, FTimerDelegate::CreateUObject(this, &AParkourGameCharacter::LogoutPlayer), AFKLogoutTime, false);
#endif
}

void AParkourGameCharacter::LogoutPlayer()
{
  static const FString LobbyMapName("entryMap");
  GEngine->SetClientTravel(GetWorld(), *LobbyMapName, TRAVEL_Absolute);
}

void AParkourGameCharacter::StandUp()
{
  if (!IsFullRagdoll() || GetVelocity().SizeSquared() > 100.0f)
  {
    return;
  }

  FVector SocketLocation = GetSkeletalMesh()->GetSocketLocation(FParkourFNames::Bone_Pelvis);
  Server_StandUp(SocketLocation);
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

	// TEMP DISABLED -- for physical animation
	//PlayerInputComponent->BindAction("RagdollArmR", IE_Pressed, this, &AParkourGameCharacter::RagdollArmR);
	//PlayerInputComponent->BindAction("RagdollArmL", IE_Pressed, this, &AParkourGameCharacter::RagdollArmL);
	//PlayerInputComponent->BindAction("RagdollLegR", IE_Pressed, this, &AParkourGameCharacter::RagdollLegR);
	//PlayerInputComponent->BindAction("RagdollLegL", IE_Pressed, this, &AParkourGameCharacter::RagdollLegL);
	//PlayerInputComponent->BindAction("RagdollTorso", IE_Pressed, this, &AParkourGameCharacter::RagdollTorso);
	//PlayerInputComponent->BindAction("RagdollLegs", IE_Pressed, this, &AParkourGameCharacter::RagdollLegs);

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

	//Roll and flip controls
	PlayerInputComponent->BindAction("Roll", IE_Pressed, this, &AParkourGameCharacter::BeginRoll);

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

bool AParkourGameCharacter::SetFullRagdoll_Validate(bool bIsFullRagdoll, bool bFromSlide) { return true; }
void AParkourGameCharacter::SetFullRagdoll_Implementation(bool bIsFullRagdoll, bool bFromSlide)
{
	bWasSliding = bFromSlide;
	m_RagdollState[(int32)EBodyPart::MAX] = bIsFullRagdoll ? 1 : 0;
  if (bIsFullRagdoll)
  {
    isRolling = false;
    OnRep_IsRolling();
  }

	OnRep_RagdollState();
	OnRagdoll.Broadcast();
}

bool AParkourGameCharacter::Server_StandUp_Validate(FVector ClientSideLocation)
{
  return true;
}

void AParkourGameCharacter::Server_StandUp_Implementation(FVector ClientSideLocation)
{    
  SetFullRagdoll(false);
  FVector AdjustedLocation = ClientSideLocation;

  if (UNavigationSystem* Nav = GetWorld()->GetNavigationSystem())
  {
    FNavLocation outLoc;
    if (Nav->ProjectPointToNavigation(ClientSideLocation, outLoc))
    {
      AdjustedLocation = outLoc.Location + (GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * FVector::UpVector);
    }
  }

  GetCapsuleComponent()->SetWorldLocation(AdjustedLocation);
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
  UClass* SpectatorClass = AParkourSpectator::StaticClass();

  if (AGameStateBase const* const GameState = GetWorld()->GetGameState())
  {
    SpectatorClass = GameState->SpectatorClass;
  }

	AParkourSpectator* SpecatorPawn = GetWorld()->SpawnActor<AParkourSpectator>(SpectatorClass, GetTransform());

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
  Hat->SetRenderCustomDepth(ShouldBeVisible);
  Hat->SetCustomDepthStencilValue(ShouldBeVisible ? 255 : 0);
}

void AParkourGameCharacter::OnJoinedTeam(AMiniGameBase* Game, AParkourGameCharacter* Player, int32 TeamID)
{
  if (Player != this) return;

  FMiniGameTeamUIInfo Info;
  Game->GetManager()->GetUIDataForTeam(TeamID, Info);

  if (Info.PlayerMaterial)
  {
    GetSkeletalMesh()->SetMaterial(0, Info.PlayerMaterial);
  }

  if (Info.HatMaterial)
  {
    Hat->SetMaterial(0, Info.HatMaterial);
  }
}

void AParkourGameCharacter::OnGameOver(AMiniGameBase* Game, EMiniGameEndReason Reason)
{
  if (IsReplayActor) return;

  GetSkeletalMesh()->SetMaterial(0, DefaultMaterial);
  Hat->SetMaterial(0, DefaultHatMaterial);
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

void AParkourGameCharacter::EnableJumping(bool Enable /*= true*/)
{
  bCanJump = Enable;
}

void AParkourGameCharacter::OnRep_RagdollState()
{
	USkeletalMeshComponent* PlayerMesh = GetSkeletalMesh();
	
	if (m_RagdollState[(int32)EBodyPart::MAX] > 0)
	{
		UCapsuleComponent* Capsule = GetCapsuleComponent();
		EnablePhysicalAnimation(false);
		PlayerMesh->ResetAllBodiesSimulatePhysics();
		GetParkourMovementComp()->SetMovementMode(MOVE_None);
		Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
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
		EnablePhysicalAnimation(true);


    if (!isRolling || !isFlipping)
    {
      FName StandUpAnimRow = ChooseStandUpAnimation(StandUpDir);

      if (StandUpAnimRow == NAME_None) return;

      USkeletalMeshComponent* PlayerMesh = GetSkeletalMesh();
      FVector SocketLocation = PlayerMesh->GetSocketLocation(UParkourHelperLibrary::GetRootBoneForBodyPart(EBodyPart::Pelvis));
      UCapsuleComponent* Capsule = GetCapsuleComponent();
      Capsule->SetWorldLocation(SocketLocation + FVector(0.0, 0.0, .0));

      PlayStandUpAnimation(StandUpAnimRow);
    }
	}
	for (int32 i = 0; i < (int32)EBodyPart::MAX; ++i)
	{
		PlayerMesh->SetAllBodiesBelowSimulatePhysics(UParkourHelperLibrary::GetRootBoneForBodyPart((EBodyPart)i), m_RagdollState[i] > 0, true);
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

    AActor* HeldBall = m_GripData[i].HeldBall.Get();
    AActor* PreviousHeldBall = m_GripData[i].PreviousHeldBall.Get();

    if (PreviousHeldBall != HeldBall)
    {
      if (PreviousHeldBall)
      {
        // drop the current ball
        DropBall(PreviousHeldBall, (EHandSideEnum)i);
        m_GripData[i].PreviousHeldBall.Reset();
      }

      if (HeldBall)
      {
        // pickup new ball
        GrabBall(HeldBall, (EHandSideEnum)i);
        m_GripData[i].PreviousHeldBall = HeldBall;
      }
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

void AParkourGameCharacter::RollRecoverWindowEnded()
{
  bCanRollRecover = false;
}

void AParkourGameCharacter::CapsuleToRagdoll()
{
	USkeletalMeshComponent* PlayerMesh = GetSkeletalMesh();
	if (m_RagdollState[(int32)EBodyPart::MAX] > 0) {
		FVector SocketLocation = PlayerMesh->GetSocketLocation(UParkourHelperLibrary::GetRootBoneForBodyPart(EBodyPart::Pelvis));
		UCapsuleComponent* Capsule = GetCapsuleComponent();

		FHitResult OutHit;
		FVector Start = SocketLocation; //(Capsule->GetComponentLocation());

		FVector DownVector = (Capsule->GetUpVector()) * -1;
		FVector End = ((DownVector * 100.f) + Start);
		FCollisionQueryParams CollisionParams;

		bool bFoundFloor;
		bFoundFloor = GetWorld()->LineTraceSingleByChannel(
			OutHit,        //result
			Start,    //start
			End, //end
			ECC_Pawn, //collision channel
			CollisionParams
		);

		if (bFoundFloor)
		{
			if ((OutHit.Distance >= 90.0))
			{
				//handle capsule position differently if sliding, for the camera's sake
				if (bWasSliding) {
					Capsule->SetWorldLocation(SocketLocation + FVector(0.0, 0.0, 88.0));
				}
				else {
					Capsule->SetWorldLocation(SocketLocation);
				}
			}
			else
			{
				Capsule->SetWorldLocation(FVector(SocketLocation.X, SocketLocation.Y, OutHit.ImpactPoint.Z +88.0));
			}
		}
		else {
			Capsule->SetWorldLocation(SocketLocation);
		}
	}
}

bool AParkourGameCharacter::Server_JoinMinigame_Validate() { return true; }
void AParkourGameCharacter::Server_JoinMinigame_Implementation()
{
	AMiniGameManager* Mgr = SingletonHelper->GetSingletonObject<AMiniGameManager>(GetWorld());
	if (!Mgr) return;

	Mgr->AddPlayerToGame(this);
}
