#include "SingletonHelper.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"

#include "../MiniGame/MiniGameManager.h"
#include "../ParkourGameCharacter.h"
#include "../Spectator/ReplayManager.h"

FSingletonHelper::FSingletonHelper()
{
}

FSingletonHelper::~FSingletonHelper()
{
}

AActor* FSingletonHelper::GetSingletonObjectByClass(UWorld* WorldPtr, UClass* Class)
{
	if (!Class) return nullptr;

	//search for the object in our existing list
	TWeakObjectPtr<AActor>* FoundObject = ObjectPtrs.Find(Class);

	if (FoundObject)
	{
		if (AActor* ExistingObject = FoundObject->Get()) return ExistingObject;
	}

  AActor* FoundActor = Static_GetSingletonObjectByClass(WorldPtr, Class);
  if (FoundActor)
    ObjectPtrs.Add(Class, FoundActor);

  return FoundActor;
}



AActor* FSingletonHelper::Static_GetSingletonObjectByClass(UWorld* WorldPtr, UClass* Class)
{
  if (!WorldPtr) return nullptr;

  // otherwise try to find the object in the world
  for (TActorIterator<AActor> It(WorldPtr, Class); It; ++It)
  {
    if (AActor* FoundActor = *It)
    {
      return FoundActor;
    }
  }

  return nullptr;
}

class AMiniGameManager* USingletonHelperLibrary::GetMiniGameManager(UObject* WorldContextObject)
{
	if(AParkourGameCharacter* Char = Cast<AParkourGameCharacter>(UGameplayStatics::GetPlayerPawn(WorldContextObject, 0)))
		return Char->SingletonHelper->GetSingletonObject<AMiniGameManager>(Char->GetWorld());

	return FSingletonHelper::Static_GetSingletonObject<AMiniGameManager>(GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::ReturnNull));
}

class AReplayManager* USingletonHelperLibrary::GetReplayManager(UObject* WorldContextObject)
{
  if (AParkourGameCharacter* Char = Cast<AParkourGameCharacter>(UGameplayStatics::GetPlayerPawn(WorldContextObject, 0)))
    return Char->SingletonHelper->GetSingletonObject<AReplayManager>(Char->GetWorld());

  return FSingletonHelper::Static_GetSingletonObject<AReplayManager>(GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::ReturnNull));
}
