#pragma once

#include "CoreMinimal.h"
#include "GameFramework/EngineMessage.h"
#include "ParkourLocalMessage.generated.h"

UCLASS()
class UParkourLocalMessage : public UEngineMessage
{
	GENERATED_BODY()

	virtual void ClientReceive(const FClientReceiveData& ClientData) const override;


private:

	void PostToasterMessage(class APlayerController* LocalPlayerController, class APlayerState* PlayerState, bool bExiting = false) const;
};
