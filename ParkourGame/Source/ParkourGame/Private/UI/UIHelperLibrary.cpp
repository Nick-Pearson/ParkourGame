#include "UIHelperLibrary.h"
#include "HttpModule.h"
#include "IHttpResponse.h"

#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerInput.h"

#define LOCTEXT_NAMESPACE "ParkourGame"

#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerInput.h"

#define LOCTEXT_NAMESPACE "ParkourGame"

FText UUIHelperLibrary::FormatToasterMessage(const FText& FormatString, const TArray<FText>& Parameters)
{
	FFormatOrderedArguments args;
	args.Reserve(Parameters.Num());

	for (const FText& param : Parameters)
	{
		args.Add(FFormatArgumentValue(param));
	}

	return FText::Format(FTextFormat(FormatString), args);
}

void UUIHelperLibrary::GetServerList(FServerListReturnEvent event){
    TSharedRef<IHttpRequest> HttpReq = FHttpModule::Get().CreateRequest();
    
    HttpReq->SetVerb("GET");
    HttpReq->SetURL("https://api.parkour.ultra-horizon.com/servers");
    
    HttpReq->OnProcessRequestComplete().BindStatic(&UUIHelperLibrary::ProcessResponse, event);
    
    HttpReq->ProcessRequest();
}

void UUIHelperLibrary::ProcessResponse(FHttpRequestPtr ReqPtr, FHttpResponsePtr Response, bool Success, FServerListReturnEvent event){
    if (!Success){
        return;
    }
    FString serverList = Response->GetContentAsString();
    event.Execute(serverList);
}

FText UUIHelperLibrary::GetDisplayStringForAction(UObject* WorldContextObject, const FName& ActionName)
{
	APlayerController* LocalPlayerController = UGameplayStatics::GetPlayerController(WorldContextObject, 0);

	if (!LocalPlayerController || !LocalPlayerController->PlayerInput) return FText::GetEmpty();

	const TArray<FInputActionKeyMapping> Keys = LocalPlayerController->PlayerInput->GetKeysForAction(ActionName);

	if (!Keys.IsValidIndex(0)) return FText::GetEmpty();

	FString ModifierString = "";

	if (Keys[0].bCmd) ModifierString.Append("Cmd ");
	if (Keys[0].bCtrl) ModifierString.Append("Ctrl ");
	if (Keys[0].bAlt) ModifierString.Append("Alt ");
	if (Keys[0].bShift) ModifierString.Append("Shift ");

	return FText::Format(LOCTEXT("ActionStringFormat", "{0}{1}"),
		FText::FromString(ModifierString),
		Keys[0].Key.GetDisplayName());
}

#undef LOCTEXT_NAMESPACE
