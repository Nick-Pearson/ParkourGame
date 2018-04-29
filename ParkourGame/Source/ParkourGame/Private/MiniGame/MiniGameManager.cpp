#include "MiniGameManager.h"

#include "../Core/ParkourGameState.h"
#include "../Core/ParkourPlayerController.h"

#include "../UI/ParkourGameHUD.h"
#include "../UI/ToasterInterface.h"
#include "../UI/UIHelperLibrary.h"

#include "../Utils/SingletonHelper.h"
#include "../Utils/ParkourFNames.h"

#include "Kismet/GameplayStatics.h"
#include "UnrealNetwork.h"

static void cmd_CreateRandom(UWorld* World)
{
  AParkourPlayerController* PlayerCtlr = Cast<AParkourPlayerController>(World->GetFirstPlayerController());
  if (PlayerCtlr) PlayerCtlr->Server_StartRandomGame();

}

FAutoConsoleCommandWithWorld CreateGameCmd(
	TEXT("Parkour.StartGame"),
	TEXT("Creates a random minigame to play if possible"),
	FConsoleCommandWithWorldDelegate::CreateStatic(cmd_CreateRandom));

AMiniGameManager::AMiniGameManager()
{
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;
	bAlwaysRelevant = true;
}

void AMiniGameManager::BeginPlay()
{
	Super::BeginPlay();
	
	if(!HasAuthority())
		OnGameOver.AddDynamic(this, &AMiniGameManager::OnGameEnded);

	SetNextGameTimer();
}

void AMiniGameManager::CreateRandomGame()
{
	if (!HasAuthority()) return;

	int32 RandomIdx = FMath::RandHelper(AvailibleMiniGames.Num() - 1);
	if (AvailibleMiniGames.IsValidIndex(RandomIdx))
		CreateGame(AvailibleMiniGames[RandomIdx]);
}

void AMiniGameManager::CreateGame(TSubclassOf<AMiniGameBase> GameClass)
{
	if (!HasAuthority() || GameClass.Get()->HasAnyClassFlags(CLASS_Abstract)) return;

	UWorld* WorldPtr = GetWorld();

	if (ActiveGame != nullptr || WorldPtr == nullptr) return;

	ActiveGame = WorldPtr->SpawnActor<AMiniGameBase>(GameClass.Get(), GetActorTransform());
	ActiveGame->InitialiseGame(this);

	if (ActiveGame->AutoJoin) {
		TArray<AActor*> AllPlayers; 
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AParkourGameCharacter::StaticClass(), AllPlayers);

		for (auto& Player : AllPlayers)
		{
			AddPlayerToGame(Cast<AParkourGameCharacter>(Player));
		}
	}

	// Set a timer to check up on this game
	GetWorld()->GetTimerManager().SetTimer(CheckGameTimer, this, &AMiniGameManager::CheckUpOnGame, GameStartTimeout);
}

bool AMiniGameManager::AddPlayerToGame(AParkourGameCharacter* Player)
{
	if (!ActiveGame || !Player) return false;

	return ActiveGame->PlayerJoinGame(Player);
}

void AMiniGameManager::RemovePlayerFromGame(AParkourGameCharacter* Player)
{
	ActiveGame->PlayerLeaveGame(Player);
	return;
}

void AMiniGameManager::GetUIDataForTeam(int32 index, FMiniGameTeamUIInfo& outInfo) const
{
	if (!TeamUIInfoTable) return;

	TArray<FName> RowNames = TeamUIInfoTable->GetRowNames();

	if (!RowNames.IsValidIndex(index)) return;

	FMiniGameTeamUIInfo* rowPtr = TeamUIInfoTable->FindRow<FMiniGameTeamUIInfo>(RowNames[index], "GetUIDataForTeam");
	outInfo = *rowPtr;
}

void AMiniGameManager::EndGame(AMiniGameBase* Game, EMiniGameEndReason Reason)
{
	if (Game != ActiveGame) return;

	Game->OnGameEnd(Reason);
	OnGameOver.Broadcast(Game, Reason);
	ActiveGame = nullptr;

  if (Reason == EMiniGameEndReason::GameWon)
  {
    GetWorld()->GetGameState<AParkourGameState>()->Net_PlayAnnouncerSound(EAnnouncerSound::Game_Over);
  }

	if (!SingleGame) SetNextGameTimer(); //
}

void AMiniGameManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMiniGameManager, ActiveGame);
}

void AMiniGameManager::CheckUpOnGame()
{
	if (!ActiveGame || ActiveGame->GameState != EMiniGameState::Pending) return;

	if (ActiveGame->CanBeStarted())
		ActiveGame->OnGameStart();
	else
		EndGame(ActiveGame, EMiniGameEndReason::NotEnoughPlayers);
}

void AMiniGameManager::SetNextGameTimer()
{
	if (!HasAuthority())
		return;

	//GetWorld()->GetTimerManager().SetTimer(NextGameTimer, this, &AMiniGameManager::CreateRandomGame, GameCreationInterval);
}

void AMiniGameManager::OnGameEnded(AMiniGameBase* Game, EMiniGameEndReason Reason)
{
	if (!Game) return;

	FName MessageType = NAME_None;

	switch (Reason)
	{
	case EMiniGameEndReason::NotEnoughPlayers:
		MessageType = "GameOver_NotEnoughPlayers";
		break;
	case EMiniGameEndReason::PlayersLeft:
		MessageType = "GameOver_PlayersLeft";
		break;
	}

	if (MessageType == NAME_None) return;

	APlayerController* LocalPlayerController = UGameplayStatics::GetPlayerController(this, 0);

	if (!LocalPlayerController) return;

	// only message players that are actually in the game
	TArray<AParkourGameCharacter*> AllPlayers;
	Game->GetAllPlayers(AllPlayers);

	if (!AllPlayers.Contains(Cast<AParkourGameCharacter>(LocalPlayerController->GetCharacter()))) return;

	AParkourGameHUD* HUDptr = Cast<AParkourGameHUD>(LocalPlayerController->GetHUD());
	UObject* Toaster = HUDptr ? HUDptr->GetToaster() : nullptr;

	if (Toaster)
	{
		IToasterInterface::Execute_AddToasterMessage(Toaster,
			MessageType,
			{Game->DisplayName});
	}
}
