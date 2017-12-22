#include "ParkourLocalMessage.h"

#include "../UI/ParkourGameHUD.h"
#include "../UI/ToasterInterface.h"

#include "GameFramework/PlayerState.h"

void UParkourLocalMessage::ClientReceive(const FClientReceiveData& ClientData) const
{
	//	Super::ClientReceive(ClientData);
	// Engine Message is not exposed in the Engine DLL so we have to do this screwed up way of calling super
	UEngineMessage::StaticClass()->GetDefaultObject<ULocalMessage>()->ClientReceive(ClientData);

	switch (ClientData.MessageIndex)
	{
	// Player entered the game
	case 1:
		PostToasterMessage(ClientData.LocalPC, ClientData.RelatedPlayerState_1);
		break;

	// Player left the game
	case 4:
		PostToasterMessage(ClientData.LocalPC, ClientData.RelatedPlayerState_1, true);
		break;
	}
}

void UParkourLocalMessage::PostToasterMessage(APlayerController* LocalPlayerController, APlayerState* PlayerState, bool bExiting /*= false*/) const
{
	if (!PlayerState || !LocalPlayerController) return;
	
	AParkourGameHUD* HUDptr = Cast<AParkourGameHUD>(LocalPlayerController->GetHUD());
	UObject* Toaster = HUDptr ? HUDptr->GetToaster() : nullptr;
	
	if (Toaster)
		IToasterInterface::Execute_AddToasterMessage(Toaster, bExiting ? TEXT("PlayerLeft") : TEXT("PlayerJoined"), { FText::FromString(PlayerState->PlayerName) });
}

