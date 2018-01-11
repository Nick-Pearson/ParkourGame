#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UIHelperLibrary.generated.h"

UCLASS()
class UUIHelperLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "HUD|Toaster")
	static FText FormatToasterMessage(const FText& FormatString, const TArray<FText>& Parameters);
	
	// returns a string with the key combination for a particular input action
	UFUNCTION(BlueprintCallable, Category = "HUD|Keys", meta = (WorldContext = "WorldContextObject"))
	static FText GetDisplayStringForAction(UObject* WorldContextObject, const FName& ActionName);

};
