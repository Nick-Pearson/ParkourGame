// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ParkourGameGameMode.generated.h"

UCLASS(minimalapi)
class AParkourGameGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AParkourGameGameMode();

	virtual FString InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal = TEXT("")) override;

};



