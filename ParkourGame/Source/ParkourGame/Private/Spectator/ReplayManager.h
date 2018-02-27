#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "ReplayManager.generated.h"

struct FPlayerKeyframe
{
	// World time in seconds
	float WorldTime;

	FTransform RootTransform;

	static void LinearInterpolate(const FPlayerKeyframe& A, const FPlayerKeyframe& B, float Alpha, FPlayerKeyframe& Result);
};

struct FPlayerReplayData
{
	// reference to the player
	FName PlayerID = NAME_None;
	TWeakObjectPtr<APawn> PlayerPawn;

	// actor replaying the player
	TWeakObjectPtr<APawn> ReplayActor;

	// index of the start of the replay keyframes
	int32 CurrentKeyframeIdx = 0;

	// start and end time of the buffer
	float StartTime = 0.0f;
	float EndTime = 0.0f;
	float LastKeyframeTime = 0.0f;

	// keyframe data
	TArray<FPlayerKeyframe> Keyframes;

	const FPlayerKeyframe* GetKeyframe(int32 index) const;
};

UCLASS()
class AReplayManager : public AInfo
{
	GENERATED_BODY()
	
public:

	AReplayManager();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	void SetRecording(bool bShouldRecord);

	void StartReplay(float ReplayDuration, float ReplayTimeScaling = 1.0f);
	void StopReplay(bool DestroyActors = true);

public:

	UPROPERTY(EditAnywhere, Category = "Replay Manager", meta = (ClampMin = "0.1"))
	float SampleInterval = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Replay Manager", meta = (ClampMin = "1.0"))
	float MaxBufferSize = 10.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Replay Manager")
	float CurrentBufferSize = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Replay Manager")
	TSubclassOf<APawn> ReplayActorClass;
private:

	void StartRecording();
	void StopRecording();

	void OnPlayerLogin(AGameModeBase* GameMode, APlayerController* NewPlayer);
	void OnPlayerLogout(AGameModeBase* GameMode, AController* Exiting);

	void RecordKeyframe(FPlayerReplayData& Player, float WorldTime);
	void CreateKeyframe(const APawn& Player, FPlayerKeyframe& outKeyframe, float WorldTime);

	void UpdateReplay(float WorldTime);
	void UpdatePlayerReplay(const FPlayerReplayData& Player, float ReplayTime);

private:

	FDelegateHandle m_PostLoginHandle;
	FDelegateHandle m_PostLogoutHandle;

	UPROPERTY(BlueprintReadOnly, Category = "Replay Manager", meta = (AllowPrivateAccess = "true"))
	bool bIsRecording = false;

	float LastKeyframeTime = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Replay Manager", meta = (AllowPrivateAccess = "true"))
	bool bIsReplaying = false;

	float ReplayStartTime = 0.0f;
	float ReplayLength = 0.0f;
	float ReplayTimeScale = 0.0f;

	TArray<FPlayerReplayData> m_KeyframeData;

};
