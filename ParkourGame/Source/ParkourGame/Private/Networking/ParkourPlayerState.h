#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "ParkourPlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnValueChangedEvent);

UCLASS()
class AParkourPlayerState : public APlayerState
{
	GENERATED_BODY()

	AParkourPlayerState();

public:
	virtual bool ShouldBroadCastWelcomeMessage(bool bExiting = false) override;

	virtual void OnRep_PlayerName() override;

	UPROPERTY(BlueprintAssignable, Category = "ParkourPlayerState")
	FOnValueChangedEvent OnPlayerNameChanged;

};
