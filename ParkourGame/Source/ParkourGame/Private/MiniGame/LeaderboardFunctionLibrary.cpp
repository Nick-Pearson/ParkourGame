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

void ULeaderboardFunctionLibrary::GetLeaderboardEntries(FGetLeaderboardSignature Callback, const FString& PlayerFilter)
{
	if (!Callback.IsBound())
	{
		UE_LOG(ParkourGame, Warning, TEXT("Failed to send Leaderboard API request as callback was unbound"));
		return;
	}

	FString Endpoint = "score";
	if (!PlayerFilter.IsEmpty())
		Endpoint = FString::Printf(TEXT("%s?player=%s"), *Endpoint, *PlayerFilter);

	MakeAPIRequest_Internal(FAPIResponseDelegate::CreateStatic(&ULeaderboardFunctionLibrary::GetLeaderboardEntries_Response, Callback), Endpoint);
}

void ULeaderboardFunctionLibrary::PostLeaderboardEntries(FPostLeaderboardSignature Callback, const TArray<FNewLeaderboardData>& Entries)
{
	// nothing to send
	if (Entries.Num() == 0) return;

	for (const FNewLeaderboardData& Entry : Entries)
	{
    if(Entry.PlayerName.IsEmpty()) continue;

    ULowEntryJsonObject* EntryObject = ULowEntryJsonObject::Create();
    ULowEntryJsonObject::SetString(EntryObject, "playerName", Entry.PlayerName);
    ULowEntryJsonObject::SetInteger(EntryObject, "goals", Entry.Goals);
    ULowEntryJsonObject::SetInteger(EntryObject, "ownGoals", Entry.OwnGoals);

    FString OutcomeString = "W";
    switch (Entry.Outcome)
    {
    case EGameOutcome::Win:
      OutcomeString = "W";
      break;
    case EGameOutcome::Loss:
      OutcomeString = "L";
      break;
    case EGameOutcome::Draw:
      OutcomeString = "D";
      break;
    }

    ULowEntryJsonObject::SetString(EntryObject, "gameOutcome", OutcomeString);

    MakeAPIRequest_Internal(FAPIResponseDelegate::CreateStatic(&ULeaderboardFunctionLibrary::PostLeaderboardEntries_Response, Callback, Entry.PlayerName), "score", ULowEntryJsonObject::ToJsonString(EntryObject, false), "POST");
	}

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

void ULeaderboardFunctionLibrary::PostLeaderboardEntries_Response(int32 HTTPStatusCode, FString Content, FPostLeaderboardSignature Callback, FString PlayerName)
{
	Callback.ExecuteIfBound(HTTPStatusCode == 200, PlayerName);
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
