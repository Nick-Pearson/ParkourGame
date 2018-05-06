#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "IHttpRequest.h"
#include "LeaderboardFunctionLibrary.generated.h"

USTRUCT(BlueprintType)
struct FLeaderboardEntry
{
	GENERATED_BODY()

public:
	FLeaderboardEntry(const FString& inName = "", const int32 inScore = 0) : 
		PlayerName(inName), Score(inScore)
	{}

	UPROPERTY(BlueprintReadWrite, Category = "LeaderboardEntry")
	FString PlayerName;

  // calculated score for this player
	UPROPERTY(BlueprintReadWrite, Category = "LeaderboardEntry")
	int32 Score;
};

UENUM(BlueprintType)
enum class EGameOutcome : uint8
{
  Win,
  Loss,
  Draw
};

// structure for data we want to send to the leaderboard
USTRUCT(BlueprintType)
struct FNewLeaderboardData
{
  GENERATED_BODY()

public:
  FNewLeaderboardData(const FString& inName = "", const int32 inGoals = 0, const int32 inOwnGoals = 0, const EGameOutcome inOutcome = EGameOutcome::Draw) :
    PlayerName(inName), Goals(inGoals), OwnGoals(inOwnGoals), Outcome(inOutcome)
  {}

  UPROPERTY(BlueprintReadWrite, Category = "NewLeaderboardData")
  FString PlayerName;

  UPROPERTY(BlueprintReadWrite, Category = "NewLeaderboardData")
  int32 Goals;

  UPROPERTY(BlueprintReadWrite, Category = "NewLeaderboardData")
  int32 OwnGoals;

  UPROPERTY(BlueprintReadWrite, Category = "NewLeaderboardData")
  EGameOutcome Outcome;
};

class AMiniGameBase;

DECLARE_DYNAMIC_DELEGATE_TwoParams(FGetLeaderboardSignature, bool, Success, const TArray<FLeaderboardEntry>&, Entries);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FPostLeaderboardSignature, bool, Success, const FString&, PlayerName);

DECLARE_DELEGATE_TwoParams(FAPIResponseDelegate, int32 /*HTTPStatusCode*/, FString /*Content*/);

UCLASS()
class ULeaderboardFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "LeaderboardFunctionLibrary")
	static void GetLeaderboardEntries(FGetLeaderboardSignature Callback, const FString& PlayerFilter);

  // post a set of items to the DB, callback will be called once per item as they are successful
	UFUNCTION(BlueprintCallable, Category = "LeaderboardFunctionLibrary")
	static void PostLeaderboardEntries(FPostLeaderboardSignature Callback, const TArray<FNewLeaderboardData>& Entries);

private:
	static void GetLeaderboardEntries_Response(int32 HTTPStatusCode, FString Content, FGetLeaderboardSignature Callback);
	static void PostLeaderboardEntries_Response(int32 HTTPStatusCode, FString Content, FPostLeaderboardSignature Callback, FString PlayerName);

	static void MakeAPIRequest_Internal(const FAPIResponseDelegate& OnAPIResponse, const FString& Endpoint, const FString& ContentString = "", const FString& Method = "GET");
	static void OnResponseCompleted(FHttpRequestPtr ReqPtr, FHttpResponsePtr Response, bool Success, FAPIResponseDelegate ResponseEvent);
};
