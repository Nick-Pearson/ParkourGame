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
  void StartGame(TSubclassOf<AMiniGameBase> GameClass);
};
