#include "ParkourPlayerState.h"

#include "ParkourLocalMessage.h"

AParkourPlayerState::AParkourPlayerState()
{
	EngineMessageClass = UParkourLocalMessage::StaticClass();
}

bool AParkourPlayerState::ShouldBroadCastWelcomeMessage(bool bExiting /*= false*/)
{
	return Super::ShouldBroadCastWelcomeMessage(bExiting) && (!bExiting || HasAuthority());
}
