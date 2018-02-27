#include "SpectatorCameraActor.h"

#include "Kismet/KismetMathLibrary.h"
#include "Camera/CameraComponent.h"

ASpectatorCameraActor::ASpectatorCameraActor()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void ASpectatorCameraActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);


	const AActor* TargetPtr = TargetActor.Get();
	if (!TargetPtr)
		return;

	const FVector TargetLoc = TargetPtr->GetActorLocation();
	const FVector CameraLoc = GetActorLocation();

	SetActorRotation(UKismetMathLibrary::FindLookAtRotation(CameraLoc, TargetLoc));
	
	if (FMath::IsNearlyZero(MinFOVDistance - MaxFOVDistance) || MinFOVDistance < MaxFOVDistance) return;

	const float Dist = (CameraLoc - TargetLoc).Size();
	const float FOVFactor = 1.0f - FMath::Clamp((Dist - MaxFOVDistance) / (MinFOVDistance - MaxFOVDistance), 0.0f, 1.0f);

	GetCameraComponent()->FieldOfView = FMath::Lerp(MinFOV, MaxFOV, FOVFactor);
}

void ASpectatorCameraActor::SetTarget(const AActor* NewTarget)
{
	TargetActor = NewTarget;

	SetActorTickEnabled(NewTarget != nullptr);
}
