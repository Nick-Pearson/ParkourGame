#include "ReplayManager.h"

#include "../Utils/ParkourGameLogging.h"
#include "../ParkourGameCharacter.h"

#include "GameFramework/GameModeBase.h"
#include "GameFramework/Controller.h"
#include "GameFramework/PlayerState.h"
#include "EngineUtils.h"
#include "Class.h"
#include "BufferArchive.h"

void FPlayerKeyframe::LinearInterpolate(const FPlayerKeyframe& A, const FPlayerKeyframe& B, float Alpha, FPlayerKeyframe& Result)
{
	Alpha = FMath::Clamp(Alpha, 0.0f, 1.0f);

  Result.WorldTime = FMath::Lerp(A.WorldTime, B.WorldTime, Alpha);
  
	Result.RootTransform.Blend(A.RootTransform, B.RootTransform, Alpha);
  Result.Velocity = FMath::Lerp(A.Velocity, B.Velocity, Alpha);

  Result.IsFullRagdoll = Alpha < 0.5f ? A.IsFullRagdoll : B.IsFullRagdoll;
  Result.IsInAir = Alpha < 0.5f ? A.IsInAir : B.IsInAir;
  Result.IsTackling = Alpha < 0.0f ? A.IsTackling : B.IsTackling;
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

  for (TActorIterator<AParkourGameCharacter> It(GetWorld()); It; ++It)
  {
    SetupNewPlayer((*It)->GetController(), (*It));
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

  WorldPtr->GetWorldSettings()->SetTimeDilation(ReplayTimeScaling);

  for (FPlayerReplayData& PlayerData : m_KeyframeData)
  {
    AActor* RealActor = PlayerData.RealActor.Get();

    if (RealActor) RealActor->SetActorHiddenInGame(true);
  }

  UpdateReplay(ReplayStartTime);

  OnReplayStarted.Broadcast();
}

void AReplayManager::StopReplay(bool DestroyActors /*= true*/)
{
	bIsReplaying = false;
  OnReplayEnded.Broadcast();

  GetWorld()->GetWorldSettings()->SetTimeDilation(1.0f);

	for (FPlayerReplayData& Player : m_KeyframeData)
	{
		AActor* LiveReplayActor = Player.ReplayActor.Get();
		AActor* RealActor = Player.RealActor.Get();

		if (RealActor) RealActor->SetActorHiddenInGame(false);

    if (DestroyActors)
    {
      if (LiveReplayActor) LiveReplayActor->Destroy();

      Player.ReplayActor.Reset();
    }
	}
}

void AReplayManager::GetAllReplayPlayers(TArray<AParkourGameCharacter*>& outActors) const
{
  if (!bIsReplaying) return;

  for (const FPlayerReplayData& Player : m_KeyframeData)
  {
    if (AParkourGameCharacter* ReplayActorPtr = Cast<AParkourGameCharacter>(Player.ReplayActor.Get()))
      outActors.Add(ReplayActorPtr);
  }
}

AActor* AReplayManager::GetReplayActorForRealActor(AActor* RealActor) const
{
  const FPlayerReplayData* PlayerDataPtr = m_KeyframeData.FindByPredicate([&](const FPlayerReplayData& Key) {
    return Key.RealActor == RealActor;
  });

  if (PlayerDataPtr) return PlayerDataPtr->ReplayActor.Get();

  return nullptr;
}

void AReplayManager::RegisterActorForReplay(AActor* Actor)
{
  // ensure no duplicates
  FPlayerReplayData* DataPtr = m_KeyframeData.FindByPredicate([&](const FPlayerReplayData& Data) {
    return Data.RealActor == Actor;
  });

  if (DataPtr) return;

  SetupNewPlayer(nullptr, Actor);
}

void AReplayManager::RemoveActorFromReplay(AActor* Actor)
{
  int32 DataIdx = m_KeyframeData.FindLastByPredicate([&](const FPlayerReplayData& Data) {
    return Data.RealActor == Actor;
  });

  if (DataIdx == INDEX_NONE) return;

  m_KeyframeData.RemoveAtSwap(DataIdx);
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

void AReplayManager::RecordKeyframe(FPlayerReplayData& Player, float WorldTime)
{
  AActor* LiveActor = Player.RealActor.Get();

	if (!LiveActor)
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

	CreateKeyframe(LiveActor, Player.Keyframes[NewKeyframeIndex], WorldTime);

  // weird bug where keyframes go out of order... remove when fixes
  Player.Keyframes.Sort([&](const FPlayerKeyframe& A, const FPlayerKeyframe& B) {
    return A.WorldTime < B.WorldTime;
  });

  Player.CurrentKeyframeIdx = 0;
}

void AReplayManager::CreateKeyframe(AActor* Actor, FPlayerKeyframe& outKeyframe, float WorldTime)
{
	outKeyframe.WorldTime = WorldTime;
	outKeyframe.RootTransform = Actor->GetActorTransform();
  
  if (AParkourGameCharacter* Player = Cast<AParkourGameCharacter>(Actor))
    Player->CreateKeyframe(outKeyframe);
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

  AActor* ReplayActor = Player.ReplayActor.Get();

	//update the state of the replay actor, if required set the real actor to hidden
	if (!ReplayActor)
	{
    AActor* RealActor = Player.RealActor.Get();
		UWorld* WorldPtr = GetWorld();

		FActorSpawnParameters Params;
    Params.ObjectFlags &= RF_Transient;
		ReplayActor = WorldPtr->SpawnActor<AActor>(RealActor->GetClass(), Params);

    if (AParkourGameCharacter* ReplayPlayer = Cast<AParkourGameCharacter>(ReplayActor))
    {
      ReplayPlayer->IsReplayActor = true;
      CastChecked<AParkourGameCharacter>(RealActor)->InitialiseReplayActor(ReplayPlayer);
    }

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

  if (AParkourGameCharacter* ReplayPlayer = Cast<AParkourGameCharacter>(ReplayActor))
    ReplayPlayer->ReplayKeyframe(InterpKeyframe);
}

void AReplayManager::SetupNewPlayer(AController* Controller, AActor* Player)
{
  // ensure no duplicates
  FPlayerReplayData* DataPtr = m_KeyframeData.FindByPredicate([&](const FPlayerReplayData& Data) {
    return Data.RealActor == Player;
  });

  if (DataPtr) return;

  FPlayerReplayData& NewEntry = m_KeyframeData[m_KeyframeData.AddDefaulted()];

  NewEntry.PlayerController = Controller;
  NewEntry.RealActor = Player;
  NewEntry.StartTime = GetWorld()->GetRealTimeSeconds();
  NewEntry.EndTime = GetWorld()->GetRealTimeSeconds();

  Player->OnEndPlay.AddDynamic(this, &AReplayManager::OnReplayActorEndPlay);
}

float AReplayManager::GetCurrentReplayTime(float WorldTime) const
{
  return  ((WorldTime - ReplayStartTime) * ReplayTimeScale) + ReplayStartTime - ReplayLength;
}

void AReplayManager::OnReplayActorEndPlay(AActor* Actor, EEndPlayReason::Type EndPlayReason)
{
  //dont bother if we are about to be destroyed too
  if (EndPlayReason == EEndPlayReason::Type::EndPlayInEditor || EndPlayReason == EEndPlayReason::Type::Quit || IsPendingKill()) return;

  RemoveActorFromReplay(Actor);
}
