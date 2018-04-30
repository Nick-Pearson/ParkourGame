#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraActor.h"
#include "SpectatorCameraActor.generated.h"

// abstract to ensure the derived blueprint is used instead
UCLASS(abstract)
class ASpectatorCameraActor : public ACameraActor
{
	GENERATED_BODY()

public:

	ASpectatorCameraActor();

	void Tick(float DeltaSeconds) override;

	void SetTarget(const AActor* NewTarget);

public:

	UPROPERTY(EditAnywhere, Category = "SpectatorCamera")
	float MaxFOV = 90.f;
	
	UPROPERTY(EditAnywhere, Category = "SpectatorCamera")
	float MaxFOVDistance = 700.0f;

	UPROPERTY(EditAnywhere, Category = "SpectatorCamera")
	float MinFOV = 40.0f;

	UPROPERTY(EditAnywhere, Category = "SpectatorCamera")
	float MinFOVDistance = 2000.0f;

  UPROPERTY(EditAnywhere, Category = "SpectatorCamera")
  float SwitchCameraDistance = 2200.0f;

  TWeakObjectPtr<class AParkourSpectator> OwningSpectator;

private:

	UPROPERTY(Transient)
	TWeakObjectPtr<const AActor> TargetActor;
};
