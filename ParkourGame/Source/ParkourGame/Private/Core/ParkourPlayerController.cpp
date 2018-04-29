#include "ParkourPlayerController.h"

#include "../MiniGame/MiniGameManager.h"
#include "../Utils/SingletonHelper.h"
#include "../Spectator/ParkourSpectator.h"

bool AParkourPlayerController::Server_StartGame_Validate(TSubclassOf<AMiniGameBase> GameClass)
{
  return true;
}

void AParkourPlayerController::Server_StartGame_Implementation(TSubclassOf<AMiniGameBase> GameClass)
{
  AMiniGameManager* Mgr = FSingletonHelper::Static_GetSingletonObject<AMiniGameManager>(GetWorld());

  if (!Mgr) return;
  
  Mgr->CreateGame(GameClass);
}

bool AParkourPlayerController::Server_StartRandomGame_Validate()
{
  return true;
}

void AParkourPlayerController::Server_StartRandomGame_Implementation()
{
  AMiniGameManager* Mgr = FSingletonHelper::Static_GetSingletonObject<AMiniGameManager>(GetWorld());

  if (!Mgr) return;

  Mgr->CreateRandomGame();
}

bool AParkourPlayerController::Server_EndCurrentGame_Validate()
{
  return true;
}

void AParkourPlayerController::Server_EndCurrentGame_Implementation()
{
  AMiniGameManager* Mgr = FSingletonHelper::Static_GetSingletonObject<AMiniGameManager>(GetWorld());

  if (!Mgr) return;

  Mgr->EndGame(Mgr->GetActiveGame(), EMiniGameEndReason::GameWon);
}

void AParkourPlayerController::Net_StartReplay_Implementation()
{
  FParkourSpectatorBroadcasts::StartActionReplay.Broadcast();
}
