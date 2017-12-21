#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "ParkourPlayerState.generated.h"

UCLASS()
class AParkourPlayerState : public APlayerState
{
	GENERATED_BODY()

	AParkourPlayerState();

public:
	virtual bool ShouldBroadCastWelcomeMessage(bool bExiting = false) override;

};
