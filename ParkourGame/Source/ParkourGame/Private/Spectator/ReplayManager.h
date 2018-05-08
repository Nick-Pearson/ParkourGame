#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "ReplayManager.generated.h"

USTRUCT()
struct FKeyframeBase
{
  GENERATED_BODY()

public:

  // World time in seconds
  float WorldTime;

  FTransform RootTransform;
};

USTRUCT(BlueprintType)
struct FPlayerKeyframe : public FKeyframeBase
{
  GENERATED_BODY()

public:

  FVector Velocity;

  int32 IsFullRagdoll;

  bool IsInAir;

  UPROPERTY(BlueprintReadWrite, Category = "PlayerKeyframe")
  bool IsTackling;

	static void LinearInterpolate(const FPlayerKeyframe& A, const FPlayerKeyframe& B, float Alpha, FPlayerKeyframe& Result);
};

class AParkourGameCharacter;
class AGameModeBase;

struct FPlayerReplayData
{
	// reference to the player
  TWeakObjectPtr<AController> PlayerController;
	TWeakObjectPtr<AActor> RealActor;

	// actor replaying the player
	TWeakObjectPtr<AActor> ReplayActor;

	// index of the start of the replay keyframes
	int32 CurrentKeyframeIdx = 0;

	// start and end time of the buffer
	float StartTime = 0.0f;
	float EndTime = 0.0f;

	// keyframe data
	TArray<FPlayerKeyframe> Keyframes;

	const FPlayerKeyframe* GetKeyframe(int32 index) const;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FReplayEvent);

UCLASS()
class AReplayManager : public AInfo
{
	GENERATED_BODY()
	
public:

	AReplayManager();

	virtual void Tick(float DeltaSeconds) override;

	void SetRecording(bool bShouldRecord);

	void StartReplay(float ReplayDuration, float ReplayTimeScaling = 1.0f);
	void StopReplay(bool DestroyActors = true);

  FORCEINLINE bool IsRecording() const { return bIsRecording; }
  FORCEINLINE bool IsReplaying() const { return bIsReplaying; }

  // get all players involved in a replay
  void GetAllReplayPlayers(TArray<AParkourGameCharacter*>& outActors) const;

  // get the replay actor for a particular real actor
  AActor* GetReplayActorForRealActor(AActor* RealActor) const;

  UFUNCTION(BlueprintCallable, Category = "ReplayManager")
  void RegisterActorForReplay(AActor* Actor);

  UFUNCTION(BlueprintCallable, Category = "ReplayManager")
  void RemoveActorFromReplay(AActor* Actor);
public:

	UPROPERTY(EditAnywhere, Category = "Replay Manager", meta = (ClampMin = "0.1"))
	float SampleInterval = 0.1f;

	UPROPERTY(EditAnywhere, Category = "Replay Manager", meta = (ClampMin = "1.0"))
	float MaxBufferSize = 10.0f;

  UPROPERTY(BlueprintAssignable, Category = "Replay Manager")
  FReplayEvent OnReplayStarted;
  
  UPROPERTY(BlueprintAssignable, Category = "Replay Manager")
  FReplayEvent OnReplayEnded;

  UPROPERTY(BlueprintAssignable, Category = "Replay Manager")
  FReplayEvent OnRecordingStarted;

  UPROPERTY(BlueprintAssignable, Category = "Replay Manager")
  FReplayEvent OnRecordingEnded;

private:

	void StartRecording();
	void StopRecording();

	void RecordKeyframe(FPlayerReplayData& Player, float WorldTime);
	void CreateKeyframe(AActor* Actor, FPlayerKeyframe& outKeyframe, float WorldTime);

	void UpdateReplay(float WorldTime);
	void UpdatePlayerReplay(FPlayerReplayData& Player, float ReplayTime);

  void SetupNewPlayer(AController* Controller, AActor* Player);

  // get the current position of the replay, represented in world time
  float GetCurrentReplayTime(float WorldTime) const;

  UFUNCTION()
  void OnReplayActorEndPlay(AActor* Actor, EEndPlayReason::Type EndPlayReason);

private:

	FDelegateHandle m_PostLoginHandle;
	FDelegateHandle m_PostLogoutHandle;

	UPROPERTY(BlueprintReadOnly, Category = "Replay Manager", meta = (AllowPrivateAccess = "true"))
	bool bIsRecording = false;

	UPROPERTY(BlueprintReadOnly, Category = "Replay Manager", meta = (AllowPrivateAccess = "true"))
	bool bIsReplaying = false;

	float ReplayStartTime = 0.0f;
	float ReplayLength = 0.0f;
	float ReplayTimeScale = 0.0f;

	TArray<FPlayerReplayData> m_KeyframeData;

};