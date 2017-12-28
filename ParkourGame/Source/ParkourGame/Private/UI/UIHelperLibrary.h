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
};
