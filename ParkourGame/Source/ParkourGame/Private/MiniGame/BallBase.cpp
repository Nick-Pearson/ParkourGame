#include "BallBase.h"

#include "../ParkourGameCharacter.h"

ABallBase::ABallBase()
{
	PrimaryActorTick.bCanEverTick = false;

}

void ABallBase::SetHeldBy(AParkourGameCharacter* Character)
{
  HeldBy = Character;
}

