#include "ParkourGameState.h"
#include "../Spectator/ReplayManager.h"
#include "../Utils/SingletonHelper.h"

void AParkourGameState::Net_PlayAnnouncerSound_Implementation(EAnnouncerSound Sound)
{
  if(!HasAuthority())
    PlayAnnouncerSound(Sound);
}

void AParkourGameState::Net_RegisterForReplay_Implementation(AActor* Actor)
{
  AReplayManager* Replay = FSingletonHelper::Static_GetSingletonObject<AReplayManager>(GetWorld());

  if (Replay)
    Replay->RegisterActorForReplay(Actor);
}
