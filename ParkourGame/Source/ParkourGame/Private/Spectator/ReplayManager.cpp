#include "ReplayManager.h"

#include "../Utils/ParkourGameLogging.h"
#include "../ParkourGameCharacter.h"

#include "GameFramework/GameModeBase.h"
#include "GameFramework/Controller.h"
#include "GameFramework/PlayerState.h"
#include "EngineUtils.h"

void FPlayerKeyframe::LinearInterpolate(const FPlayerKeyframe& A, const FPlayerKeyframe& B, float Alpha, FPlayerKeyframe& Result)
{
	Alpha = FMath::Clamp(Alpha, 0.0f, 1.0f);

	Result.RootTransform.Blend(A.RootTransform, B.RootTransform, Alpha);
	Result.WorldTime = FMath::Lerp(A.WorldTime, B.WorldTime, Alpha);
}

const FPlayerKeyframe* FPlayerReplayData::GetKeyframe(int32 index) const
{
	if (index < 0 || index > Keyframes.Num())
		return nullptr;

	return &Keyframes[(index + CurrentKeyframeIdx) % Keyframes.Num()];
}

AReplayManager::AReplayManager()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
}

void AReplayManager::BeginPlay()
{
	FGameModeEvents::GameModePostLoginEvent.AddUObject(this, &AReplayManager::OnPlayerLogin);
	FGameModeEvents::GameModeLogoutEvent.AddUObject(this, &AReplayManager::OnPlayerLogout);
}

void AReplayManager::Tick(float DeltaSeconds)
{
	if (!bIsRecording && !bIsReplaying)
	{
		SetActorTickEnabled(false);
		return;
	}

	UWorld* WorldPtr = GetWorld();
	if (!ensureMsgf(WorldPtr, TEXT("AReplayManager::Tick Failed to get world ptr")))
	{
		bIsRecording = false;
		bIsReplaying = false;
	}

	float CurrentTime = WorldPtr->GetRealTimeSeconds();

	if (bIsRecording)
	{
		for (FPlayerReplayData& PlayerData : m_KeyframeData)
		{
      if (PlayerData.EndTime + SampleInterval <= CurrentTime)
      {
				RecordKeyframe(PlayerData, CurrentTime);
			}
		}
	}

	if (bIsReplaying)
	{
		UpdateReplay(CurrentTime);
	}
}

void AReplayManager::SetRecording(bool bShouldRecord)
{
	if (bShouldRecord && !bIsRecording)
	{
		StartRecording();
	}
	else if (!bShouldRecord && bIsRecording)
	{
		StopRecording();
	}
}

void AReplayManager::StartReplay(float ReplayDuration, float ReplayTimeScaling /*= 1.0f*/)
{
	if (ReplayDuration < 0.1f || ReplayDuration > MaxBufferSize)
	{
		UE_LOG(ParkourGame, Warning, TEXT("[ReplayManager] Replay not started: Invalid Duration or Time Scale specified (%.2f, %.2f)"), ReplayDuration, ReplayTimeScaling);
		return;
	}

	if (bIsReplaying)
		StopReplay(false);

	UWorld* WorldPtr = GetWorld();

	if (!WorldPtr)
		return;

	bIsReplaying = true;
	ReplayStartTime = WorldPtr->GetRealTimeSeconds();
	ReplayTimeScale = ReplayTimeScaling;
	ReplayLength = ReplayDuration;

  UpdateReplay(ReplayStartTime);

  OnReplayStarted.Broadcast();
}

void AReplayManager::StopReplay(bool DestroyActors /*= true*/)
{
	bIsReplaying = false;
  OnReplayEnded.Broadcast();

	if (DestroyActors)
	{
		for (FPlayerReplayData& Player : m_KeyframeData)
		{
			AActor* LiveReplayActor = Player.ReplayActor.Get();
			APawn* RealActor = Player.PlayerPawn.Get();

			if (RealActor) RealActor->SetActorHiddenInGame(false);
			if (LiveReplayActor) LiveReplayActor->Destroy();

			Player.ReplayActor.Reset();
		}
	}
}

void AReplayManager::GetAllReplayActors(TArray<const AActor*>& outActors) const
{
  if (!bIsReplaying) return;

  for (const FPlayerReplayData& Player : m_KeyframeData)
  {
    if (AActor* ReplayActorPtr = Player.ReplayActor.Get())
      outActors.Add(ReplayActorPtr);
  }
}

void AReplayManager::StartRecording()
{
	bIsRecording = true;
  m_KeyframeData.Empty();

  // add all current players to the recording
  for (TActorIterator<AParkourGameCharacter> It(GetWorld()); It; ++It)
  {
    SetupNewPlayer((*It)->GetController(), (*It));
  }

	SetActorTickEnabled(true);
  OnRecordingStarted.Broadcast();
}

void AReplayManager::StopRecording()
{
	bIsRecording = false;

  m_KeyframeData.Empty();

  OnRecordingEnded.Broadcast();
}

void AReplayManager::OnPlayerLogin(AGameModeBase* GameMode, APlayerController* NewPlayer)
{
  if (!bIsRecording || !NewPlayer || !NewPlayer->PlayerState) return;

  AParkourGameCharacter* Character = Cast<AParkourGameCharacter>(NewPlayer->GetPawn());
  if (!Character) return;

  SetupNewPlayer(NewPlayer, Character);
}

void AReplayManager::OnPlayerLogout(AGameModeBase* GameMode, AController* Exiting)
{
  int32 PlayerDataIdx = m_KeyframeData.FindLastByPredicate([&](const FPlayerReplayData& Data) {
    return Data.PlayerController == Exiting;
  });

  if (PlayerDataIdx == INDEX_NONE) return;

  m_KeyframeData.RemoveAtSwap(PlayerDataIdx);
}

void AReplayManager::RecordKeyframe(FPlayerReplayData& Player, float WorldTime)
{
	const APawn* LivePlayer = Player.PlayerPawn.Get();

	if (!LivePlayer)
		return;

	const float LocalBufferSize = Player.EndTime - Player.StartTime;
	const float LastInterval = WorldTime - Player.EndTime;

	if (!ensureMsgf(LastInterval > 0.0f, TEXT("Keyframe data has become corrupted")))
		return;

	int32 NewKeyframeIndex;

  bool CanCreateNewKeyframe = true;

  // if we are replaying ensure we don't trash the data we need to do a replay
  const float ReplayTime = GetCurrentReplayTime(WorldTime);
  if (bIsReplaying && Player.Keyframes.Num() > 1 && Player.StartTime < ReplayTime)
  {
    const FPlayerKeyframe* Keyframe = Player.GetKeyframe(Player.CurrentKeyframeIdx + 1);

    if (Keyframe && Keyframe->WorldTime > ReplayTime)
    {
      CanCreateNewKeyframe = false;
    }
  }

	// either re-use an old keyframe or add a new one
	if (CanCreateNewKeyframe && LocalBufferSize + LastInterval > MaxBufferSize)
	{
		NewKeyframeIndex = Player.CurrentKeyframeIdx;
		++Player.CurrentKeyframeIdx;

		if (Player.CurrentKeyframeIdx >= Player.Keyframes.Num())
			Player.CurrentKeyframeIdx = 0;

		Player.StartTime = Player.Keyframes[Player.CurrentKeyframeIdx].WorldTime;
	}
	else
	{
		NewKeyframeIndex = Player.Keyframes.AddDefaulted();
	}

	//populate the keyframe
	Player.EndTime = WorldTime;

	CreateKeyframe(*LivePlayer, Player.Keyframes[NewKeyframeIndex], WorldTime);

  // weird bug where keyframes go out of order... remove when fixes
  Player.Keyframes.Sort([&](const FPlayerKeyframe& A, const FPlayerKeyframe& B) {
    return A.WorldTime < B.WorldTime;
  });

  Player.CurrentKeyframeIdx = 0;
}

void AReplayManager::CreateKeyframe(const APawn& Player, FPlayerKeyframe& outKeyframe, float WorldTime)
{
	outKeyframe.WorldTime = WorldTime;
	outKeyframe.RootTransform = Player.GetActorTransform();
}

void AReplayManager::UpdateReplay(float WorldTime)
{
	const float ReplayTime = GetCurrentReplayTime(WorldTime);

	if (ReplayTime - ReplayStartTime > 0.0f)
	{
		StopReplay();
		return;
	}

	for (FPlayerReplayData& Player : m_KeyframeData)
	{
		UpdatePlayerReplay(Player, ReplayTime);
	}
}

void AReplayManager::UpdatePlayerReplay(FPlayerReplayData& Player, float ReplayTime)
{
	//check if the player has data for this point in time
	bool HasValidData = ReplayTime < Player.EndTime && ReplayTime > Player.StartTime;

  if (!HasValidData)
    return;

	APawn* ReplayActor = Player.ReplayActor.Get();

	//update the state of the replay actor, if required set the real actor to hidden
	if (!ReplayActor)
	{
		APawn* RealActor = Player.PlayerPawn.Get();
		UClass* ReplayActorClassPtr = ReplayActorClass.Get();
		UWorld* WorldPtr = GetWorld();

		if (!ensureMsgf(ReplayActorClassPtr, TEXT("[ReplayManager] Replay Actor Class was invalid")) || !RealActor || !WorldPtr)
			return;

		FActorSpawnParameters Params;
		ReplayActor = WorldPtr->SpawnActor<APawn>(ReplayActorClassPtr, Params);
    Player.ReplayActor = ReplayActor;

		RealActor->SetActorHiddenInGame(true);
	}

	const FPlayerKeyframe* Key1 = nullptr;
	const FPlayerKeyframe* Key2 = nullptr;

	//linear interpolate between two closest keyframes and apply to replay actor
	for (int32 i = 0; i < Player.Keyframes.Num(); ++i)
	{
		const FPlayerKeyframe* CurrKey = Player.GetKeyframe(i);

		if(!ensure(CurrKey))
			continue;

		if (!Key1 || (Key1->WorldTime < CurrKey->WorldTime && CurrKey->WorldTime < ReplayTime))
		{
			Key1 = CurrKey;
			Key2 = Player.GetKeyframe(i + 1);
		}
	}

	if (!ensure(Key1 && Key2))
		return;
	
	float Alpha = (ReplayTime - Key1->WorldTime) / (Key2->WorldTime - Key1->WorldTime);

  FPlayerKeyframe InterpKeyframe;
	FPlayerKeyframe::LinearInterpolate(*Key1, *Key2, Alpha, InterpKeyframe);

	//apply the keyframe to the replay actor
	ReplayActor->SetActorTransform(InterpKeyframe.RootTransform);
}

void AReplayManager::SetupNewPlayer(AController* Controller, AParkourGameCharacter* Player)
{
  FPlayerReplayData& NewEntry = m_KeyframeData[m_KeyframeData.AddDefaulted()];

  NewEntry.PlayerController = Controller;
  NewEntry.PlayerPawn = Player;
  NewEntry.StartTime = GetWorld()->GetRealTimeSeconds();
  NewEntry.EndTime = GetWorld()->GetRealTimeSeconds();
}

float AReplayManager::GetCurrentReplayTime(float WorldTime) const
{
  return  ((WorldTime - ReplayStartTime) * ReplayTimeScale) + ReplayStartTime - ReplayLength;
}
