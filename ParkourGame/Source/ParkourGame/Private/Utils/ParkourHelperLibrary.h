#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "ParkourTypes.h"
#include "ParkourHelperLibrary.generated.h"

UCLASS()
class UParkourHelperLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	// Returns the root of the chain of bones for a body part - e.g. for an arm this would be the shoulder
	UFUNCTION(BlueprintPure, Category = "Parkour|Skeleton")
	static FName GetRootBoneForBodyPart(EBodyPart Part);

	// Returns the end of the chain of bones for a body part - e.g. for an arm this would be the hand
	UFUNCTION(BlueprintPure, Category = "Parkour|Skeleton")
	static FName GetEndBoneForBodyPart(EBodyPart Part);

	// Returns the set of (one or more) bones that make up a body part
	UFUNCTION(BlueprintPure, Category = "Parkour|Skeleton")
	static void GetBoneChainForBodyPart(EBodyPart Part, TArray<FName>& outChain);



  UFUNCTION(BlueprintPure, Category = "Lobby")
    static bool IsValidPlayerName(const FString& Name);
};
