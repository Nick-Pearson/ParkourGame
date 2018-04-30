#pragma once

#include "CoreMinimal.h"
#include "../Utils/SingletonHelper.h"
#include "GameFramework/SpectatorPawn.h"
#include "ParkourSpectator.generated.h"

class ASpectatorCameraActor;

// set of events to broadcasts events between different spectators in the same game
// broadcast these from NetMulticast events
namespace FParkourSpectatorBroadcasts
{
  DECLARE_MULTICAST_DELEGATE_OneParam(FStartActionReplayEvent, UWorld*);

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
	void SetViewedActor(AActor* NewActor);

	// This logic randomly switches between players every set 10 seconds
	float TargetChangeTime = 10.0f;
	FTimerHandle TargetChangeHandle;

  float CameraChangeTime = 4.0f;
  FTimerHandle CameraChangeHandle;

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

  void SwitchCamera();

public:

  UPROPERTY(EditAnywhere, Category = "ParkourSpectator")
  TSubclassOf<ASpectatorCameraActor> SpectatorCameraClass;

  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ParkourSpectator")
  bool IsAutoCam = false;

private:

  UFUNCTION()
  void TeamScoreUpdated(AMiniGameBase* Game, int32 TeamID);

  void InitialiseActionReplay();
  void StartPlayingReplay(UWorld* World);

	ASpectatorCameraActor* GetBestCamera() const;

	UPROPERTY(BlueprintReadOnly, Category = "ParkourSpectator", meta = (AllowPrivateAccess = "true"))
	AActor* ViewedActor = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "ParkourSpectator", meta = (AllowPrivateAccess = "true"))
	ASpectatorCameraActor* CurrentCamera = nullptr;

  // set to trye when the current camera should be destoryed after use
  bool TempCamera = false;

  TWeakObjectPtr<AActor> ReplayTarget;

	UPROPERTY(Transient)
	TArray<ASpectatorCameraActor*> AllCameras;

  FSingletonHelper SingletonHelper;
};
