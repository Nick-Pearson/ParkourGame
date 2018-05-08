#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "ParkourGameState.generated.h"

UENUM(BlueprintType)
enum class EAnnouncerSound : uint8
{
  Begin,
  Game_Over,
  Prepare,

  MAX
};

UCLASS()
class AParkourGameState : public AGameStateBase
{
	GENERATED_BODY()
	
public:

  UFUNCTION(BlueprintCallable, NetMulticast, Unreliable, Category = "Audio")
  void Net_PlayAnnouncerSound(EAnnouncerSound Sound);

  UFUNCTION(NetMulticast, Reliable)
  void Net_RegisterForReplay(AActor* Actor);

  UFUNCTION(BlueprintImplementableEvent, Category = "Audio")
  void PlayAnnouncerSound(EAnnouncerSound Sound);

};
