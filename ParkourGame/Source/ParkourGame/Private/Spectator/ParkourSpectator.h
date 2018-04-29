#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SpectatorPawn.h"
#include "ParkourSpectator.generated.h"

class ASpectatorCameraActor;

// set of events to broadcasts events between different spectators in the same game
// broadcast these from NetMulticast events
namespace FParkourSpectatorBroadcasts
{
  DECLARE_MULTICAST_DELEGATE(FStartActionReplayEvent);

  extern FStartActionReplayEvent StartActionReplay;
};


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

  UFUNCTION()
	void TargetNewPlayer();

  UFUNCTION(BlueprintCallable, Category = "ParkourSpectator")
  void BeginAutoCam();

  UFUNCTION(BlueprintCallable, Category = "ParkourSpectator")
  void StartGame(TSubclassOf<AMiniGameBase> GameClass);

  UFUNCTION(BlueprintCallable, Category = "ParkourSpectator")
  void EndCurrentGame();
  
  UFUNCTION(BlueprintCallable, Category = "ParkourSpectator")
  void StartActionReplay();

  void OpenControls();

private:

  void InitialiseActionReplay();
  void StartPlayingReplay();

	void SwitchCamera();

	ASpectatorCameraActor* GetBestCamera() const;

	UPROPERTY(BlueprintReadOnly, Category = "ParkourSpectator", meta = (AllowPrivateAccess = "true"))
	const AActor* ViewedActor;

	UPROPERTY(BlueprintReadOnly, Category = "ParkourSpectator", meta = (AllowPrivateAccess = "true"))
	ASpectatorCameraActor* CurrentCamera;

	UPROPERTY(Transient)
	TArray<ASpectatorCameraActor*> AllCameras;

};
