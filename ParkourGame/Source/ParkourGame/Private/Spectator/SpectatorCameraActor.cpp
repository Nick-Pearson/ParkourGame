#include "SpectatorCameraActor.h"

#include "Kismet/KismetMathLibrary.h"
#include "Camera/CameraComponent.h"
#include "ParkourSpectator.h"

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

  if (const AParkourGameCharacter* Char = Cast<AParkourGameCharacter>(TargetPtr))
  {
    // dont follow glitchy ragdoll capsule
    if (Char->IsFullRagdoll()) return;
  }

	SetActorRotation(UKismetMathLibrary::FindLookAtRotation(CameraLoc, TargetLoc));

  const float Dist = (CameraLoc - TargetLoc).Size();

  if (Dist > SwitchCameraDistance)
  {
    OwningSpectator->SwitchCamera();
  }

  GetCameraComponent()->PostProcessSettings.DepthOfFieldFocalDistance = Dist;

	if (FMath::IsNearlyZero(MinFOVDistance - MaxFOVDistance) || MinFOVDistance < MaxFOVDistance) return;

	const float FOVFactor = 1.0f - FMath::Clamp((Dist - MaxFOVDistance) / (MinFOVDistance - MaxFOVDistance), 0.0f, 1.0f);

	GetCameraComponent()->FieldOfView = FMath::Lerp(MinFOV, MaxFOV, FOVFactor);
}

void ASpectatorCameraActor::SetTarget(const AActor* NewTarget)
{
	TargetActor = NewTarget;

	SetActorTickEnabled(NewTarget != nullptr);
}
