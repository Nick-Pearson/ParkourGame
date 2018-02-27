#include "ReplayManager.h"

#include "../Utils/ParkourGameLogging.h"

#include "GameFramework/GameModeBase.h"

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

	float CurrentTime = WorldPtr->GetTimeSeconds();

	if (bIsRecording)
	{
		if (LastKeyframeTime + SampleInterval <= CurrentTime)
		{
			for (FPlayerReplayData& PlayerData : m_KeyframeData)
			{
				RecordKeyframe(PlayerData, CurrentTime);
			}

			LastKeyframeTime = CurrentTime;
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
	ReplayStartTime = WorldPtr->GetTimeSeconds();
	ReplayTimeScale = ReplayTimeScaling;
	ReplayLength = ReplayDuration;
}

void AReplayManager::StopReplay(bool DestroyActors /*= true*/)
{
	bIsReplaying = false;

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

void AReplayManager::StartRecording()
{
	bIsRecording = true;
	SetActorTickEnabled(true);
}

void AReplayManager::StopRecording()
{
	bIsRecording = false;
}

void AReplayManager::OnPlayerLogin(AGameModeBase* GameMode, APlayerController* NewPlayer)
{
	if (bIsRecording)
	{
		FPlayerReplayData& NewEntry = m_KeyframeData[m_KeyframeData.AddDefaulted()];

		NewEntry.PlayerID = NAME_None; // @TODO: Get a player ID from some networking manager?
		NewEntry.PlayerPawn = NewPlayer->GetPawn();
	}
}

void AReplayManager::OnPlayerLogout(AGameModeBase* GameMode, AController* Exiting)
{
	if (bIsRecording)
	{
		// Exiting->GetPawn();  @TODO: Remove by player ID
	}
}

void AReplayManager::RecordKeyframe(FPlayerReplayData& Player, float WorldTime)
{
	const APawn* LivePlayer = Player.PlayerPawn.Get();

	if (!ensure(LivePlayer))
		return;

	const float LocalBufferSize = Player.EndTime - Player.StartTime;
	const float LastInterval = WorldTime - Player.EndTime;

	if (!ensureMsgf(LastInterval > 0.0f, TEXT("Keyframe data has become corrupted")))
		return;

	int32 NewKeyframeIndex;

	// either re-use an old keyframe or add a new one
	if (LocalBufferSize + LastInterval > MaxBufferSize)
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
}

void AReplayManager::CreateKeyframe(const APawn& Player, FPlayerKeyframe& outKeyframe, float WorldTime)
{
	outKeyframe.WorldTime = WorldTime;
	outKeyframe.RootTransform = Player.GetActorTransform();
}

void AReplayManager::UpdateReplay(float WorldTime)
{
	const float ReplayTime = ((WorldTime - ReplayStartTime) * ReplayTimeScale) + ReplayStartTime;

	if (ReplayTime - ReplayStartTime > ReplayLength)
	{
		StopReplay();
		return;
	}

	for (const FPlayerReplayData& Player : m_KeyframeData)
	{
		UpdatePlayerReplay(Player, ReplayTime);
	}
}

void AReplayManager::UpdatePlayerReplay(const FPlayerReplayData& Player, float ReplayTime)
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

	if (!Key1 || !Key2)
		return;

	FPlayerKeyframe InterpKeyframe;
	float Alpha = (ReplayTime - Key1->WorldTime) / (Key2->WorldTime - Key1->WorldTime);

	FPlayerKeyframe::LinearInterpolate(*Key1, *Key2, Alpha, InterpKeyframe);

	//apply the keyframe to the replay actor
	ReplayActor->SetActorTransform(InterpKeyframe.RootTransform);
}
