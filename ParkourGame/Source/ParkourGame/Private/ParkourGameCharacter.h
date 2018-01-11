// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"
#include "Math/UnrealMath.h"
#include "ParkourMesh.h"
#include "Utils/ParkourTypes.h"
#include "ParkourGameCharacter.generated.h"


UENUM(BlueprintType)
enum class EHandSideEnum : uint8
{ //enum for which hand is being used for calculations
	HS_Right	UMETA(DisplayName = "Right"),
	HS_Left		UMETA(DisplayName = "Left")
};

class USpringArmComponent;
class UCameraComponent;
class USkeletalMeshComponent;
class UConstraintManager;

UCLASS(config=Game)
class AParkourGameCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	/* Skel mesh */
	UPROPERTY(Transient)
	USkeletalMeshComponent* SkeletalMesh;

	/* Constraint Manager */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UConstraintManager* ConstraintManager;

public:
	AParkourGameCharacter();
  
	virtual void BeginPlay();
	virtual void EndPlay(EEndPlayReason::Type Reason);

	TSharedPtr<class FSingletonHelper> SingletonHelper;

	/*Include Handside enum for hand targeting calculations*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Enum)
	EHandSideEnum HandSideEnum;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSet<AParkourMesh*> NearbyParkourObjects;
  
	virtual void PostInitializeComponents() override;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	UFUNCTION()
	void BeginOverlap(AActor* OverlappedActor, AActor* OtherActor);
	UFUNCTION()
	void EndOverlap(AActor* OverlappedActor, AActor* OtherActor);

protected:

	virtual void Tick(float DeltaSeconds);

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

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

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	FORCEINLINE USkeletalMeshComponent* GetSkeletalMesh() const { return SkeletalMesh; }

	UFUNCTION(Server, Reliable, WithValidation)
	void SetRagdollOnBodyPart(EBodyPart Part, bool bNewRagdoll);

	UFUNCTION(Server, Reliable, WithValidation)
	void SetFullRagdoll(bool bIsFullRagdoll);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_JoinMinigame();

private:

	// Function called when the client recieves an update to the ragdoll state
	UFUNCTION()
	void OnRep_RagdollState();

	//set this variable to be replicated to all clients
	// array of bools indicating if that body part is in ragdoll, has extra value at the end for full body ragdoll
	UPROPERTY(Transient, ReplicatedUsing = OnRep_RagdollState)
	uint32 m_RagdollState[(int32)EBodyPart::MAX + 1];
};

