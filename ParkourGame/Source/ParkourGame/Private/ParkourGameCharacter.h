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

USTRUCT(BlueprintType)
struct FGripData
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly, Category = "GripData")
	bool isGripping;

  UPROPERTY(BlueprintReadOnly, Category = "GripData")
  TWeakObjectPtr<AActor> HeldBall;

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

public:
	AParkourGameCharacter(const FObjectInitializer& ObjectInitializer);
  
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;
	virtual void EndPlay(EEndPlayReason::Type Reason) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	TSharedPtr<class FSingletonHelper> SingletonHelper;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSet<AParkourMesh*> NearbyParkourObjects;

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physics")
	float BodyMass = 75.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ObjectDetection")
	float ObjectDetectionRadius = 200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	class UDataTable* FootstepAudioTable;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
  class UDataTable* StandUpAnimationTable;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Minigame")
  class UMaterialInterface* DefaultMaterial;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Minigame")
  TSubclassOf<AActor> BallClass;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Minigame")
  float BallPickupDistance = 200.0f;

  // time a player can be AFK before they are automatically logged out
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Minigame")
  float AFKLogoutTime = 120.0f;

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
  
  void PlayStandUpAnimation();
  FName ChooseStandUpAnimation(EStandUpDirection Direction) const;
  void ResetStandupAnim();

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	FORCEINLINE USkeletalMeshComponent* GetSkeletalMesh() const { return SkeletalMesh; }

	FORCEINLINE UPhysicalAnimationComponent* GetPhysicalAnimation() const { return PhysicalAnimation; }

	FORCEINLINE UParkourMovementComponent* GetParkourMovementComp() const { return MovementComp; }

	FORCEINLINE UTextRenderComponent* GetPlayerNameTag() const { return PlayerNameTag; }

	UFUNCTION(Server, Reliable, WithValidation)
	void SetRagdollOnBodyPart(EBodyPart Part, bool bNewRagdoll);

	UFUNCTION(BlueprintCallable, Category = "Physics", Server, Reliable, WithValidation)
	void SetFullRagdoll(bool bIsFullRagdoll, bool bFromSlide = false);

	UFUNCTION(BlueprintPure, Category = "Physics")
	bool IsFullRagdoll() const;

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

  UFUNCTION()
  void OnRep_StandUpAnimRow();

  // which animation from the table we should play to stand up
  UPROPERTY(ReplicatedUsing = OnRep_StandUpAnimRow)
  FName StandUpAnimRow = NAME_None;

  FTimerHandle ResetStandupHandle;

	UPROPERTY(Transient)
	class AParkourPlayerState* ParkourPlayerState;

  bool bCanJump = true;
  bool bWasSliding = false;
};

