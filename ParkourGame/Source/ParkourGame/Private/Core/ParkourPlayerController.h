#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ParkourPlayerController.generated.h"

class AMiniGameBase;

UCLASS()
class AParkourPlayerController : public APlayerController
{
	GENERATED_BODY()

public:

  // Spectator Broadcast Functions
  // (have to be here as spectators have no net role in a build)


  UFUNCTION(Server, Unreliable, WithValidation)
  void Server_StartGame(TSubclassOf<AMiniGameBase> GameClass);

  UFUNCTION(Server, Unreliable, WithValidation)
  void Server_StartRandomGame();

  UFUNCTION(Server, Unreliable, WithValidation)
  void Server_EndCurrentGame();

  UFUNCTION(NetMulticast, Unreliable)
  void Net_StartReplay();

  virtual void SetPawn(APawn* InPawn) override;

protected:
  virtual void BeginPlay() override;

};
