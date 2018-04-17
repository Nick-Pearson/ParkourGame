#include "ParkourGameState.h"

void AParkourGameState::Net_PlayAnnouncerSound_Implementation(EAnnouncerSound Sound)
{
  if(!HasAuthority())
    PlayAnnouncerSound(Sound);
}
