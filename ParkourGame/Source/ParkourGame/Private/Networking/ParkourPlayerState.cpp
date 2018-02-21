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

void AParkourPlayerState::OnRep_PlayerName()
{
	Super::OnRep_PlayerName();

	OnPlayerNameChanged.Broadcast();
}
