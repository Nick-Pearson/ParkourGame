#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SpectatorPawn.h"
#include "ParkourSpectator.generated.h"

class ASpectatorCameraActor;

UCLASS()
class AParkourSpectator : public ASpectatorPawn
{
	GENERATED_BODY()

public:
  AParkourSpectator();

	void BeginPlay() override;

	void PossessedBy(class AController* NewController) override;

	FORCEINLINE const AActor* GetViewedActor() { return ViewedActor; }
	
	UFUNCTION(BlueprintCallable, Category = "ParkourSpectator")
	void SetViewedActor(const AActor* NewActor);

	// This logic randomly switches between players every set 10 seconds TODO: Add to this logic for when interesting events occur
	float TargetChangeTime = 10.0f;
	FTimerHandle TargetChangeHandle;

	void TargetRandomPlayer();

  UFUNCTION(BlueprintCallable, Category = "ParkourSpectator")
  void BeginAutoCam();

  UFUNCTION(BlueprintCallable, Category = "ParkourSpectator", Server, Unreliable, WithValidation)
  void StartGame(TSubclassOf<AMiniGameBase> GameClass);

  void OpenControls();

private:


	void SwitchCamera();

	ASpectatorCameraActor* GetBestCamera() const;

	UPROPERTY(BlueprintReadOnly, Category = "ParkourSpectator", meta = (AllowPrivateAccess = "true"))
	const AActor* ViewedActor;

	UPROPERTY(BlueprintReadOnly, Category = "ParkourSpectator", meta = (AllowPrivateAccess = "true"))
	ASpectatorCameraActor* CurrentCamera;

	UPROPERTY(Transient)
	TArray<ASpectatorCameraActor*> AllCameras;

};
