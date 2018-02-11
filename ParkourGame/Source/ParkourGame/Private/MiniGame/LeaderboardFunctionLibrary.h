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

	UPROPERTY(BlueprintReadWrite, Category = "LeaderboardEntry")
	int32 Score;
};

// NOTE:: This corresponds to an index on the leaderboard 
// Do not reorder or otherwise change the mapping of games to integers without updating the server as well
UENUM(BlueprintType)
enum class EMinigameFilter : uint8
{
	CaptureTheFlag = 0,

	None = 255
};

class AMiniGameBase;

DECLARE_DYNAMIC_DELEGATE_TwoParams(FGetLeaderboardSignature, bool, Success, const TArray<FLeaderboardEntry>&, Entries);
DECLARE_DYNAMIC_DELEGATE_OneParam(FPostLeaderboardSignature, bool, Success);

DECLARE_DELEGATE_TwoParams(FAPIResponseDelegate, int32 /*HTTPStatusCode*/, FString /*Content*/);

UCLASS()
class ULeaderboardFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "LeaderboardFunctionLibrary")
	static void GetLeaderboardEntries(FGetLeaderboardSignature Callback, EMinigameFilter MinigameFilter = EMinigameFilter::None);

	UFUNCTION(BlueprintCallable, Category = "LeaderboardFunctionLibrary")
	static void PostLeaderboardEntries(FPostLeaderboardSignature Callback, const TArray<FLeaderboardEntry>& Entries, EMinigameFilter MinigameID);

private:
	static void GetLeaderboardEntries_Response(int32 HTTPStatusCode, FString Content, FGetLeaderboardSignature Callback);
	static void PostLeaderboardEntries_Response(int32 HTTPStatusCode, FString Content, FPostLeaderboardSignature Callback);

	static void MakeAPIRequest_Internal(const FAPIResponseDelegate& OnAPIResponse, const FString& Endpoint, const FString& ContentString = "", const FString& Method = "GET");
	static void OnResponseCompleted(FHttpRequestPtr ReqPtr, FHttpResponsePtr Response, bool Success, FAPIResponseDelegate ResponseEvent);
};
