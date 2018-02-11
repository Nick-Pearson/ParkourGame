#include "LeaderboardFunctionLibrary.h"

#include "../Utils/ParkourGameLogging.h"

#include "HttpModule.h"
#include "IHttpResponse.h"

#include "LowEntryJsonLibrary.h"
#include "LowEntryJsonArray.h"
#include "LowEntryJsonObject.h"

namespace
{
	const FString APIRoot = "https://api.parkour.ultra-horizon.com";
	const FString AuthorizationToken = "#87WebnOJ6^zTCX4";
};

void ULeaderboardFunctionLibrary::GetLeaderboardEntries(FGetLeaderboardSignature Callback, EMinigameFilter MinigameFilter /*= EMinigameFilter::None*/)
{
	if (!Callback.IsBound())
	{
		UE_LOG(ParkourGame, Warning, TEXT("Failed to send Leaderboard API request as callback was unbound"));
		return;
	}

	FString Endpoint = "score";
	if (MinigameFilter != EMinigameFilter::None)
		Endpoint = FString::Printf(TEXT("%s?minigameId=%d"), *Endpoint, (int32)MinigameFilter);

	MakeAPIRequest_Internal(FAPIResponseDelegate::CreateStatic(&ULeaderboardFunctionLibrary::GetLeaderboardEntries_Response, Callback), Endpoint);
}

void ULeaderboardFunctionLibrary::PostLeaderboardEntries(FPostLeaderboardSignature Callback, const TArray<FLeaderboardEntry>& Entries, EMinigameFilter MinigameID)
{
	// nothing to send
	if (Entries.Num() == 0) return;

	if (MinigameID == EMinigameFilter::None)
	{
		UE_LOG(ParkourGame, Warning, TEXT("Failed to send Leaderboard API request as no minigame was specified"));
		return;
	}

	int32 SessionID = 0; // TODO: Generate a session ID

	ULowEntryJsonArray* ScoresArray = ULowEntryJsonArray::Create();

	for (const FLeaderboardEntry& Entry : Entries)
	{
		ULowEntryJsonArray* EntryArray = ULowEntryJsonArray::Create();
		ULowEntryJsonArray::AddString(ScoresArray, Entry.PlayerName);
		ULowEntryJsonArray::AddInteger(ScoresArray, Entry.Score);

		ULowEntryJsonArray::AddJsonArray(ScoresArray, EntryArray);
	}

	ULowEntryJsonObject* ContainerObject = ULowEntryJsonObject::Create();
	ULowEntryJsonObject::SetJsonArray(ContainerObject, "scores", ScoresArray);
	ULowEntryJsonObject::SetInteger(ContainerObject, "minigameId", SessionID);
	ULowEntryJsonObject::SetInteger(ContainerObject, "sessionId", (int32)MinigameID);

	MakeAPIRequest_Internal(FAPIResponseDelegate::CreateStatic(&ULeaderboardFunctionLibrary::PostLeaderboardEntries_Response, Callback), "score", ULowEntryJsonObject::ToJsonString(ContainerObject, false), "POST");
}

void ULeaderboardFunctionLibrary::GetLeaderboardEntries_Response(int32 HTTPStatusCode, FString Content, FGetLeaderboardSignature Callback)
{
	if (HTTPStatusCode != 200)
	{
		Callback.Execute(false, {});
		return;
	}

	ULowEntryJsonObject* DummyPtr = nullptr;
	ULowEntryJsonArray* ArrObj = nullptr;

	ELowEntryJsonParseResult Result;
	ULowEntryJsonLibrary::Json_ParseJsonString(Content, DummyPtr, ArrObj, Result);

	if (!ArrObj || Result != ELowEntryJsonParseResult::FoundArray)
	{
		Callback.Execute(false, {});
		return;
	}

	int32 ArrayLength = ULowEntryJsonArray::Length(ArrObj);
	TArray<FLeaderboardEntry> ReturnValue;
	ReturnValue.Reserve(ArrayLength);

	for (int32 i = 0; i < ArrayLength; ++i)
	{
		ELowEntryJsonValueAndTypeFound Result;
		ULowEntryJsonArray* Val = ULowEntryJsonArray::GetJsonArray(ArrObj, i, Result);
		
		if (!Val || Result != ELowEntryJsonValueAndTypeFound::Success || ULowEntryJsonArray::Length(Val) < 2) continue;
		
		FString PlayerName = ULowEntryJsonArray::GetString(Val, 0, Result);
		if(Result != ELowEntryJsonValueAndTypeFound::Success) continue;

		int32 Score = ULowEntryJsonArray::GetInteger(Val, 1, Result);
		if (Result != ELowEntryJsonValueAndTypeFound::Success) continue;

		ReturnValue.Add(FLeaderboardEntry(PlayerName, Score));
	}

	Callback.Execute(true, ReturnValue);
}

void ULeaderboardFunctionLibrary::PostLeaderboardEntries_Response(int32 HTTPStatusCode, FString Content, FPostLeaderboardSignature Callback)
{
	Callback.ExecuteIfBound(HTTPStatusCode == 200);
}

void ULeaderboardFunctionLibrary::MakeAPIRequest_Internal(const FAPIResponseDelegate& OnAPIResponse, const FString& Endpoint, const FString& ContentString /*= ""*/, const FString& Method /*= "GET"*/)
{
	TSharedRef<IHttpRequest> HttpReq = FHttpModule::Get().CreateRequest();

	HttpReq->SetVerb(Method);
	HttpReq->SetURL(APIRoot / Endpoint);
	HttpReq->SetHeader("Content-Type", "application/json");
	HttpReq->SetHeader("Authorization", AuthorizationToken);
	HttpReq->SetContentAsString(ContentString);

	HttpReq->OnProcessRequestComplete().BindStatic(&ULeaderboardFunctionLibrary::OnResponseCompleted, OnAPIResponse);

	HttpReq->ProcessRequest();
}

void ULeaderboardFunctionLibrary::OnResponseCompleted(FHttpRequestPtr ReqPtr, FHttpResponsePtr Response, bool Success, FAPIResponseDelegate ResponseEvent)
{
	// no callback - nothing to do
	if (!ResponseEvent.IsBound())
		return;
	
	if (!Success)
	{
		ResponseEvent.Execute(0, "");
	}

	// TODO: Check for some common API errors here

	ResponseEvent.Execute(Response->GetResponseCode(), Response->GetContentAsString());
}
