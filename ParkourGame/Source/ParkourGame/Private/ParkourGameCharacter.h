// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"
#include "Core/ParkourPlayerController.h"
#include "Math/UnrealMath.h"
#include "ParkourMesh.h"
#include "Utils/ParkourTypes.h"
#include "Physics/PushSpringSystem.h"
#include "DrawDebugHelpers.h"
#include "Engine/DataTable.h"
#include "Spectator/ReplayManager.h"
#include "ParkourGameCharacter.generated.h"


class USpringArmComponent;
class UCameraComponent;
class USkeletalMeshComponent;
class UConstraintManager;
class UPhysicalAnimationComponent;
class USimpleSpringSystem;
class USpringSystem;
class UParkourMovementComponent;
class USphereComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FParkourGameCharacterEvent);

UENUM(BlueprintType)
enum class EMiniGameEndReason : uint8
{
  // Not enough players joined to make the game valid to start
  NotEnoughPlayers,

  // A team has won the game
  GameWon,

  // Some players have dropped out of the game making it invalid
  PlayersLeft,

  MAX			UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EHandSideEnum : uint8
{ //enum for which hand is being used for calculations
	HS_Right	UMETA(DisplayName = "Right"),
	HS_Left		UMETA(DisplayName = "Left"),

	MAX			UMETA(Hidden)
};

// direction the player ragdoll is facing at the start of a stand up animation
UENUM(BlueprintType)
enum class EStandUpDirection : uint8
{
  FaceDown,
  FaceUp,
};


USTRUCT(BlueprintType)
struct FStandUpMontageRow : public FTableRowBase
{
  GENERATED_BODY()

public:

  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "StandUpMontageRow")
  EStandUpDirection Direction;

  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "StandUpMontageRow")
  UAnimMontage* Montage;
};


USTRUCT(BlueprintType)
struct FParkourTarget
{

  GENERATED_BODY()

public:

  UPROPERTY(BlueprintReadOnly, Category = "ParkourTarget")
	FVector Target;

  UPROPERTY(BlueprintReadOnly, Category = "ParkourTarget")
	FRotator Rot;

  UPROPERTY(BlueprintReadOnly, Category = "ParkourTarget")
	FVector GripTarget;

  UPROPERTY(BlueprintReadOnly, Category = "ParkourTarget")
	FVector VaultTarget;
};

class ABallBase;

USTRUCT(BlueprintType)
struct FGripData
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly, Category = "GripData")
	bool isGripping;

  UPROPERTY(BlueprintReadOnly, Category = "GripData")
  TWeakObjectPtr<ABallBase> HeldBall;

  UPROPERTY(BlueprintReadOnly, Category = "GripData", NotReplicated)
  TWeakObjectPtr<AActor> PreviousHeldBall;

	UPROPERTY(BlueprintReadOnly, Category = "GripData")
	FVector gripTarget;

	UPROPERTY(BlueprintReadOnly, Category = "GripData", NotReplicated)
	USpringSystem* ArmSpring;
};

USTRUCT(BlueprintType)
struct FVaultData
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly, Category = "VaultData")
	bool isVaulting;

	UPROPERTY(BlueprintReadOnly, Category = "VaultData")
	FVector vaultTarget;

	UPROPERTY(BlueprintReadOnly, Category = "VaultData", NotReplicated)
	UPushSpringSystem* ArmSpring;
};

USTRUCT(BlueprintType)
struct FPushData
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly, Category = "PushData")
	bool isPushing;

	UPROPERTY(BlueprintReadOnly, Category = "PushData")
	FVector pushTarget;

	UPROPERTY(BlueprintReadOnly, Category = "PushData", NotReplicated)
	UPushSpringSystem* ArmSpring;
};

UCLASS(config=Game)
class AParkourGameCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	/* Skel mesh */
	UPROPERTY(Transient)
	USkeletalMeshComponent* SkeletalMesh;

	/* Constraint Manager */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Physics", meta = (AllowPrivateAccess = "true"))
	UConstraintManager* ConstraintManager;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Physics", meta = (AllowPrivateAccess = "true"))
	UPhysicalAnimationComponent* PhysicalAnimation;
	
	UPROPERTY(BlueprintReadOnly, Category = "Physics", meta = (AllowPrivateAccess = "true"))
	USimpleSpringSystem* LegSpring;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Physics", meta = (AllowPrivateAccess = "true"))
	UParkourMovementComponent* MovementComp;

	UPROPERTY(VisibleAnywhere, Category = "ObjectDetection")
	USphereComponent* ObjectDetectionSphere;

	UPROPERTY(EditAnywhere, Category = "ObjectDetection")
	class UTextRenderComponent* PlayerNameTag;

	UPROPERTY(VisibleAnywhere, Category = "Audio")
	USphereComponent* FootSphereL;
	
	UPROPERTY(VisibleAnywhere, Category = "Audio")
	USphereComponent* FootSphereR;

	UPROPERTY(VisibleAnywhere, Category = "Physics", meta = (AllowPrivateAccess = "true"))
	FVector2D RollMagnitude;

  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Display", meta = (AllowPrivateAccess = "true"))
  class UStaticMeshComponent* Hat;


public:
	AParkourGameCharacter(const FObjectInitializer& ObjectInitializer);
  
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;
	virtual void EndPlay(EEndPlayReason::Type Reason) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
  virtual FVector GetVelocity() const override;

	TSharedPtr<class FSingletonHelper> SingletonHelper;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSet<AParkourMesh*> NearbyParkourObjects;


  virtual void FellOutOfWorld(const class UDamageType& dmgType) override;

public:

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera")
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera")
	float BaseLookUpRate;

	UFUNCTION()
	void BeginOverlap(AActor* OverlappedActor, AActor* OtherActor);
	UFUNCTION()
	void EndOverlap(AActor* OverlappedActor, AActor* OtherActor);

	// the physics subsystems will be ticked as if the game is running at this framerate
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physics")
	int32 PhysicsSubstepTargetFramerate = 120;

	UFUNCTION(BlueprintCallable, Category = "Physics")
	void EndVaultAnim();

  UFUNCTION(BlueprintPure, Category = "Role")
  bool IsAutonomousProxy() const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physics")
	float BodyMass = 75.0f;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physics")
  float GetUpDelay = 0.3f;
  
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physics")
  float RollDuration = 0.9f;

  // time after ragdolling from a fall when you can use roll to recover faster
  UPROPERTY(EditAnywhere, Category = "Physics")
  float RollRecoverWindow = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ObjectDetection")
	float ObjectDetectionRadius = 200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	class UDataTable* FootstepAudioTable;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
  class UDataTable* StandUpAnimationTable;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Minigame")
  class UMaterialInterface* DefaultMaterial;
  
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Minigame")
  class UMaterialInterface* DefaultHatMaterial;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Minigame")
  TSubclassOf<ABallBase> BallClass;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Minigame")
  float BallPickupDistance = 200.0f;

  // time a player can be AFK before they are automatically logged out
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Minigame")
  float AFKLogoutTime = 120.0f;

  UPROPERTY(BlueprintReadOnly, Category = "Replay")
  bool IsReplayActor = false;

  UPROPERTY(BlueprintReadOnly, Category = "Replay")
  bool Replay_IsInAir = false;

  UPROPERTY(BlueprintReadOnly, Category = "Replay")
  FVector Replay_Velocity;

  UPROPERTY(BlueprintReadOnly, Category = "Physics")
  float DefaultCapsuleRadius;

  UPROPERTY(BlueprintReadOnly, Category = "Physics")
  float DefaultCapsuleHalfHeight;

	//EVENTS

	UPROPERTY(BlueprintAssignable, Category = "ParkourGameCharacter|Events")
		FParkourGameCharacterEvent OnRagdoll;



protected:

	virtual void Tick(float DeltaSeconds);

	UFUNCTION(BlueprintImplementableEvent, Category = "Physics", meta = (DisplayName = "Subtick Physics"))
	void BPE_SubtickPhysics(float DeltaSeconds);

	void SubtickPhysics(float DeltaSeconds);

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	void Jump() override;

	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	void RagdollBody();

	void RagdollArmR();

	void RagdollArmL();

	void RagdollLegR();

	void RagdollLegL();

	void RagdollTorso();

	void RagdollLegs();

  // This function uses multiple traces in front of the player and outputs a list of the trace results that pass criteria
	void GetVisualTargets(const FVector& Start, TArray<FHitResult>& outVHits) const;

  // parses a set of hit results and outputs an array of ParkourTargets from those hits
	void GetParkourTargets(TArray<FParkourTarget>& outPTargs, const TArray<FHitResult>& VHits) const;

  bool GetParkourTargetClosestTo(const FVector& Location, FParkourTarget& outTarget);

	void BeginGrip(EHandSideEnum Hand);
	void EndGrip(EHandSideEnum Hand);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_Vault(EHandSideEnum Hand);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_BeginGrip(EHandSideEnum Hand);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_EndGrip(EHandSideEnum Hand);

  UFUNCTION(BlueprintCallable, Category = "BallHandling")
  bool Server_GrabNearbyBall(EHandSideEnum Hand);

  UFUNCTION(BlueprintCallable, Category = "BallHandling")
  bool Server_DropBall(EHandSideEnum Hand);

  UFUNCTION(BlueprintImplementableEvent)
  void GrabBall(AActor* Ball, EHandSideEnum Hand);

  UFUNCTION(BlueprintImplementableEvent)
  void DropBall(AActor* Ball, EHandSideEnum Hand);

  UFUNCTION()
  void OnRagdollEvent();
  
//	void BeginPush(EHandSideEnum Hand);
//	void EndPush(EHandSideEnum Hand);

  void ResetAFKTimer();
  void LogoutPlayer();
  FTimerHandle AFKTimerHandle;

	void StandUp();

	void CapsuleToRagdoll();


	UFUNCTION(BlueprintCallable, Category = "Minigame")
	void JoinMinigame() { ResetAFKTimer(); Server_JoinMinigame(); }

	/**
	* Returns position to direct a given hand with IK
	* @param handSide	Used to determine whether the function is calculating for the left or right hand
	*/

	UFUNCTION(BlueprintCallable, meta = (DeprecatedFunction))
	FVector GetParkourHandTarget(EHandSideEnum handSide);

  UFUNCTION(BlueprintCallable)
  bool GetParkourTarget(EHandSideEnum HandSide, FParkourTarget& Target);

	/**
	* Returns pointer to closest ParkourMesh object
	*/
	UFUNCTION(BlueprintCallable)
	AParkourMesh* GetNearestParkourObject();

	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

	UFUNCTION()
	void PlayFootstepL(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void PlayFootstepR(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void PlayFootstepSound(int32 SoundType, bool isLeft);

	virtual void OnRep_PlayerState() override;

	UFUNCTION()
	void OnPlayerNameChanged();
  
  void PlayStandUpAnimation(const FName& StandUpAnimRow);
  FName ChooseStandUpAnimation(EStandUpDirection Direction) const;
  void ResetStandupAnim();

  UFUNCTION(BlueprintNativeEvent, Category = "Replay", meta = (DisplayName = "CreateKeyframe"))
  FPlayerKeyframe BP_CreateKeyframe(const FPlayerKeyframe& Keyframe);

  UFUNCTION(BlueprintNativeEvent, Category = "Replay", meta = (DisplayName = "ReplayKeyframe"))
  bool BP_ReplayKeyframe(const FPlayerKeyframe& Keyframe);

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	FORCEINLINE USkeletalMeshComponent* GetSkeletalMesh() const { return SkeletalMesh; }

	FORCEINLINE UPhysicalAnimationComponent* GetPhysicalAnimation() const { return PhysicalAnimation; }

	FORCEINLINE UParkourMovementComponent* GetParkourMovementComp() const { return MovementComp; }

	FORCEINLINE UTextRenderComponent* GetPlayerNameTag() const { return PlayerNameTag; }

  FORCEINLINE UStaticMeshComponent* GetHat() const { return Hat; }

	UFUNCTION(Server, Reliable, WithValidation)
	void SetRagdollOnBodyPart(EBodyPart Part, bool bNewRagdoll);


  UFUNCTION(Server, Reliable, WithValidation)
  void Server_StandUp(FVector ClientSideLocation);

	UFUNCTION(BlueprintCallable, Category = "Physics", Server, Reliable, WithValidation)
	void SetFullRagdoll(bool bIsFullRagdoll, bool bFromSlide = false);

	UFUNCTION(BlueprintPure, Category = "Physics")
	bool IsFullRagdoll() const;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	float Time_to_Floor() const;

  UFUNCTION(BlueprintCallable, Category = "Physics")
	void Tick_Roll(float DeltaSeconds);

  UFUNCTION(BlueprintCallable, Category = "Physics")
	void Roll_Start();

  void BeginRoll();
  UFUNCTION(Server, WithValidation, Reliable)
  void Server_BeginRoll();

  void EnableRolling(bool Enable) { bRollingEnabled = Enable; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physics", Replicated)
	bool isVaulting = false;


  // only runs on servre
  UFUNCTION(BlueprintCallable, Category = "Physics")
  void StartRollRecoverTimer();

  UFUNCTION(BlueprintNativeEvent, Category = "Physics")
  void OnRep_IsRolling();

	UPROPERTY(BlueprintReadOnly, Category = "Physics", ReplicatedUsing = OnRep_IsRolling)
	bool isRolling = false;

	float MoveForward_mag = 0.0f;
	float MoveRight_mag = 0.0f;
	      
  UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Physics")
  void OnRep_IsFlipping();

	UPROPERTY(BlueprintReadWrite, Category = "Physics",  ReplicatedUsing = OnRep_IsFlipping)
	bool isFlipping = false;

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_JoinMinigame();

	// EDITOR ONLY
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_BecomeSpectator();

	UFUNCTION(BlueprintPure, Category = "Input")
	void GetGripData(EHandSideEnum Hand, FGripData& Data) const;

	UFUNCTION(BlueprintPure, Category = "Input")
	void GetVaultData(EHandSideEnum Hand, FVaultData& Data) const;

	UFUNCTION(BlueprintCallable, Category = "Rendering")
	void SetVisibleInXRay(bool ShouldBeVisible);

	//UFUNCTION(BlueprintPure, Category = "Input")
	//void GetPushData(EHandSideEnum Hand, FPushData& Data) const;

  UFUNCTION()
  void OnJoinedTeam(AMiniGameBase* Game, AParkourGameCharacter* Player, int32 TeamID);

  UFUNCTION()
  void OnGameOver(AMiniGameBase* Game, EMiniGameEndReason Reason);
  
  UFUNCTION(BlueprintCallable, Category = "CharacterState")
  void EnableJumping(bool Enable = true);

  bool CreateKeyframe(FPlayerKeyframe& Keyframe);

  bool ReplayKeyframe(const FPlayerKeyframe& Keyframe);

  UFUNCTION(BlueprintNativeEvent, Category = "Replay")
  void InitialiseReplayActor(AParkourGameCharacter* ReplayActor);
  
  UFUNCTION(BlueprintPure, Category = "BallHandling")
  bool HasBall() const;

private:
	AParkourPlayerController* GetParkourPlayerController() const;

	// returns true if the location is within a 90 radius in front of the player
	bool IsWithinFieldOfView(const FVector& Location) const;

	// indicates the seconds until the next physics sub tick
	float m_PhysicsClock = 0.0f;
	FVector SubphysicsVelocity = FVector::ZeroVector;

	void EnablePhysicalAnimation(bool Enable = true);

	// Function called when the client recieves an update to the ragdoll state
	UFUNCTION()
	void UpdateVault(EHandSideEnum Hand);
	
	// Function called when the client recieves an update to the ragdoll state
	UFUNCTION()
	void OnRep_RagdollState();

	//set this variable to be replicated to all clients
	// array of bools indicating if that body part is in ragdoll, has extra value at the end for full body ragdoll
	UPROPERTY(Transient, ReplicatedUsing = OnRep_RagdollState)
	uint32 m_RagdollState[(int32)EBodyPart::MAX + 1];

	UFUNCTION()
	void OnRep_GripData();

	UFUNCTION()
	void OnRep_VaultData();

	UPROPERTY(Transient, ReplicatedUsing = OnRep_GripData)
	FGripData m_GripData[(int32)EHandSideEnum::MAX];

	UPROPERTY(Transient, ReplicatedUsing = OnRep_VaultData)
	FVaultData m_VaultData[(int32)EHandSideEnum::MAX];

	UPROPERTY(Transient)
	FPushData m_PushData[(int32)EHandSideEnum::MAX];

  FTimerHandle ResetStandupHandle;

  FTimerHandle AutoStandUpHandle;

  FTimerHandle RollRecoverHandle;
  bool bCanRollRecover = false;

  void RollRecoverWindowEnded();

	UPROPERTY(Transient)
	class AParkourPlayerState* ParkourPlayerState;

  bool bCanJump = true;
  bool bWasSliding = false;

  float StopRollingDelay = 1.0f;
  float AutoStandUpTime = 0.0f;

  bool bRollingEnabled = true;
};

