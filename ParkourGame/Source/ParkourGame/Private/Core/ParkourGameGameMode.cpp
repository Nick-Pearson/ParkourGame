#include "ParkourGameGameMode.h"

#include "GameFramework/Pawn.h"
#include "UObject/ConstructorHelpers.h"
#include "GameFramework/PlayerState.h"

#include "ParkourPlayerController.h"
#include "../UI/ParkourGameHUD.h"
#include "../Networking/ParkourPlayerState.h"

AParkourGameGameMode::AParkourGameGameMode()
{
	// find our blueprinted versions
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (ensureMsgf(PlayerPawnBPClass.Class, TEXT("Unable to resolve player blueprint class")))
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	static ConstructorHelpers::FClassFinder<AHUD> HUDBPClass(TEXT("/Game/HUD/ParkourGameHUD_BP"));
	if (ensureMsgf(HUDBPClass.Class, TEXT("Unable to resolve HUD blueprint class")))
	{
		HUDClass = HUDBPClass.Class;
	}

	PlayerControllerClass = AParkourPlayerController::StaticClass();
	PlayerStateClass = AParkourPlayerState::StaticClass();
}

FString AParkourGameGameMode::InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal /*= TEXT("")*/)
{
	FString msg = Super::InitNewPlayer(NewPlayerController, UniqueId, Options, Portal);
	return msg;
}

