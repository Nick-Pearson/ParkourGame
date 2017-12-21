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
	
};
