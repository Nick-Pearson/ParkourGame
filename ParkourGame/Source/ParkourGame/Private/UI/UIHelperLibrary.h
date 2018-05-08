#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "IHttpRequest.h"
#include "UIHelperLibrary.generated.h"


DECLARE_DYNAMIC_DELEGATE_OneParam( FServerListReturnEvent, const FString&, serverList );


UCLASS()
class UUIHelperLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "HUD|Toaster")
	static FText FormatToasterMessage(const FText& FormatString, const TArray<FText>& Parameters);
	
    UFUNCTION(BlueprintCallable, Category = "HUD|Toaster")
    static void GetServerList(FServerListReturnEvent event);
    static void ProcessResponse(FHttpRequestPtr ReqPtr, FHttpResponsePtr Response, bool Success, FServerListReturnEvent event);
	
	// returns a string with the key combination for a particular input action
	UFUNCTION(BlueprintCallable, Category = "HUD|Keys", meta = (WorldContext = "WorldContextObject"))
	static FText GetDisplayStringForAction(UObject* WorldContextObject, const FName& ActionName);
};
