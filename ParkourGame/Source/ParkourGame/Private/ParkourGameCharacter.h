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



UENUM(BlueprintType)
enum class EHandSideEnum : uint8
{ //enum for which hand is being used for calculations
	HS_Right	UMETA(DisplayName = "Right"),
	HS_Left		UMETA(DisplayName = "Left"),

	MAX			UMETA(Hidden)
};


struct EParkourTarget
{

public:

	FVector Target;
	FRotator Rot;
	FVector GripTarget;
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
	FVector gripTarget;

	UPROPERTY(BlueprintReadOnly, Category = "GripData", NotReplicated)
	USpringSystem* ArmSpring;
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
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Physics", meta = (AllowPrivateAccess = "true"))
	USimpleSpringSystem* LegSpring;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Physics", meta = (AllowPrivateAccess = "true"))
	UParkourMovementComponent* MovementComp;

	UPROPERTY(VisibleAnywhere, Category = "ObjectDetection")
	USphereComponent* ObjectDetectionSphere;

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

	int GetVisualTargets(FHitResult* VHit);
	void GetParkourTargets(EParkourTarget* PTarg, FHitResult* VHit, int vc);

	void BeginGrip(EHandSideEnum Hand);
	void EndGrip(EHandSideEnum Hand);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_BeginGrip(EHandSideEnum Hand);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_EndGrip(EHandSideEnum Hand);

	void BeginPush(EHandSideEnum Hand);
	void EndPush(EHandSideEnum Hand);

	void StandUp();

	void CapsuleToRagdoll();

	void JoinMinigame() { Server_JoinMinigame(); }

	/**
	* Returns position to direct a given hand with IK
	* @param handSide	Used to determine whether the function is calculating for the left or right hand
	*/

	UFUNCTION(BlueprintCallable)
	FVector GetParkourHandTarget(EHandSideEnum handSide);

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

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	FORCEINLINE USkeletalMeshComponent* GetSkeletalMesh() const { return SkeletalMesh; }

	FORCEINLINE UPhysicalAnimationComponent* GetPhysicalAnimation() const { return PhysicalAnimation; }

	FORCEINLINE UParkourMovementComponent* GetParkourMovementComp() const { return MovementComp; }

	UFUNCTION(Server, Reliable, WithValidation)
	void SetRagdollOnBodyPart(EBodyPart Part, bool bNewRagdoll);

	UFUNCTION(BlueprintCallable, Category = "Physics", Server, Reliable, WithValidation)
	void SetFullRagdoll(bool bIsFullRagdoll);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_JoinMinigame();

	UFUNCTION(BlueprintPure, Category = "Input")
	void GetGripData(EHandSideEnum Hand, FGripData& Data) const;

	UFUNCTION(BlueprintCallable, Category = "Rendering")
	void SetVisibleInXRay(bool ShouldBeVisible);

	UFUNCTION(BlueprintPure, Category = "Input")
	void GetPushData(EHandSideEnum Hand, FPushData& Data) const;

private:
	AParkourPlayerController* GetParkourPlayerController();

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

	UPROPERTY(Transient, ReplicatedUsing = OnRep_GripData)
	FGripData m_GripData[(int32)EHandSideEnum::MAX];

	UPROPERTY(Transient)
	FPushData m_PushData[(int32)EHandSideEnum::MAX];
};

