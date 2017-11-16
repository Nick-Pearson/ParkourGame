#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ConstraintManager.generated.h"


// @TODO: have a lookup table of constraint profile data we can reference for different situations

//typedef FGuid FConstraintID;

class UPhysicsConstraintComponent;

struct FManagedPhysicsConstraint
{
public:
	FGuid ConstraintID;

	TWeakObjectPtr<UPhysicsConstraintComponent> ConstraintComp;

	FName TargetBone = NAME_None;

	TWeakObjectPtr<USceneComponent> OtherComp;
	FName OtherBone = NAME_None;
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UConstraintManager : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UConstraintManager();

	// Called when the game starts
	virtual void BeginPlay() override;

public:

	UFUNCTION(BlueprintCallable, Category = "ConstraintManager", meta = (AutoCreateRefTerm="Bone,OtherBone"))
	FGuid CreateConstraint(const FName& Bone, UPrimitiveComponent* OtherComp, const FName& OtherBone = NAME_None);

	UFUNCTION(BlueprintCallable, Category = "ConstraintManager")
	void DestroyConstraint(const FGuid& Constraint);

	template<typename Func>
	void GetConstraintsByPredicate(Func Predicate, TArray<FGuid>& outConstraints) const
	{
		for (const FManagedPhysicsConstraint& Constraint : m_ActiveConstraints)
		{
			if (Predicate(Constraint))
				outConstraints.Add(Constraint.ConstraintID);
		}
	}

	UFUNCTION(BlueprintCallable, Category = "ConstraintManager", meta = (AutoCreateRefTerm="Bone"))
	void GetConstraintByTargetBone(const FName& Bone, TArray<FGuid>& outConstraints) const;

private:

	USkeletalMeshComponent* GetSkeletalMeshComp();

	TWeakObjectPtr<USkeletalMeshComponent> m_SkeletalMeshComp;
	TArray<FManagedPhysicsConstraint> m_ActiveConstraints;
};
