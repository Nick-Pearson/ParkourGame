#include "ParkourPlayerController.h"

#include "../MiniGame/MiniGameManager.h"
#include "../Utils/SingletonHelper.h"

bool AParkourPlayerController::StartGame_Validate(TSubclassOf<AMiniGameBase> GameClass)
{
  return true;
}

void AParkourPlayerController::StartGame_Implementation(TSubclassOf<AMiniGameBase> GameClass)
{
  AMiniGameManager* Mgr = FSingletonHelper::Static_GetSingletonObject<AMiniGameManager>(GetWorld());

  if (!Mgr) return;

  Mgr->CreateGame(GameClass);
}