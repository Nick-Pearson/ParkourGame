#include "ConstraintManager.h"

#include "PhysicsEngine/PhysicsConstraintActor.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"


// Sets default values for this component's properties
UConstraintManager::UConstraintManager()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void UConstraintManager::BeginPlay()
{
	Super::BeginPlay();

	
}

FGuid UConstraintManager::CreateConstraint(const FName& Bone, UPrimitiveComponent* OtherComp, const FName& OtherBone /*= NAME_None*/)
{
	UWorld* WorldPtr = GetWorld();

	if (!OtherComp || Bone == NAME_None || !WorldPtr)
		return FGuid();

	// Create constraint comp
	USkeletalMeshComponent* SkelMeshComp = GetSkeletalMeshComp();
	UPhysicsConstraintComponent* NewConstraintComp = NewObject<UPhysicsConstraintComponent>(OtherComp->GetOwner());

	if (!ensure(NewConstraintComp) || !ensure(SkelMeshComp))
		return FGuid();

	// Setup the constraint actor
	FConstraintInstance ConstraintInstance;
	NewConstraintComp->ConstraintInstance = ConstraintInstance;

	NewConstraintComp->SetWorldLocation(OtherComp->GetOwner()->GetActorLocation());
	NewConstraintComp->AttachToComponent (OtherComp, FAttachmentTransformRules::KeepWorldTransform, OtherBone);
	NewConstraintComp->SetConstrainedComponents(OtherComp, OtherBone, SkelMeshComp, Bone);

	// @TODO : Bind to constraint broken event and handle that case

	// Add to our list of managed constraints
	FManagedPhysicsConstraint& NewConstraint = m_ActiveConstraints[m_ActiveConstraints.AddDefaulted()];

	NewConstraint.ConstraintID.NewGuid();
	NewConstraint.ConstraintComp = NewConstraintComp;
	NewConstraint.TargetBone = Bone;
	NewConstraint.OtherComp = OtherComp;
	NewConstraint.OtherBone = OtherBone;

	return NewConstraint.ConstraintID;
}

void UConstraintManager::DestroyConstraint(const FGuid& Constraint)
{
	int32 RemoveIndex = m_ActiveConstraints.FindLastByPredicate([&](const FManagedPhysicsConstraint& QueryConstraint) {
		return QueryConstraint.ConstraintID == Constraint;
	});

	if (RemoveIndex == INDEX_NONE)
		return;

	UPhysicsConstraintComponent* ConstraintCompPtr = m_ActiveConstraints[RemoveIndex].ConstraintComp.Get();

	if (ConstraintCompPtr)
	{
		// @TODO Unbind from constrain broken event
		ConstraintCompPtr->BreakConstraint();
		ConstraintCompPtr->DestroyComponent();
	}

	m_ActiveConstraints.RemoveAtSwap(RemoveIndex);
}

void UConstraintManager::GetConstraintByTargetBone(const FName& Bone, TArray<FGuid>& outConstraints) const
{
	GetConstraintsByPredicate([&](const FManagedPhysicsConstraint& Constraint) {
		return Constraint.TargetBone == Bone;
	}, outConstraints);
}

USkeletalMeshComponent* UConstraintManager::GetSkeletalMeshComp()
{
	USkeletalMeshComponent* CompPtr = m_SkeletalMeshComp.Get();

	if (CompPtr) return CompPtr;

	return Cast<USkeletalMeshComponent>(GetOwner()->GetComponentByClass(USkeletalMeshComponent::StaticClass()));
}
