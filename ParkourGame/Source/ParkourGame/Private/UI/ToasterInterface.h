#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ToasterInterface.generated.h"

USTRUCT(BlueprintType)
struct FToasterMessageInfo : public FTableRowBase
{
	GENERATED_BODY()
	
public:

	// format for the toaster messages, text such as {0}, {1} will be replaced with the parameter passed at that index
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ToasterMessageInfo")
	FText MessageFormat;

	// can multiple versions of this message type be stacked into a single larger message
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ToasterMessageInfo")
	bool bCanBeStacked;

	// format for a message that is stacked, in this case {0} is the number of stacked messages, custom parameters are numbered from {1}
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ToasterMessageInfo", meta = (EditCondition = "bCanBeStacked"))
	FText StackedMessageFormat;
};

USTRUCT(BlueprintType)
struct FToasterQueuedMessage
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ToasterQueuedMessage")
	float MessageTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ToasterQueuedMessage")
	FName Type;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ToasterQueuedMessage")
	TArray<FText> Parameters;
};

UINTERFACE(MinimalAPI)
class UToasterInterface : public UInterface
{
	GENERATED_BODY()
};

class IToasterInterface
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "HUD|Toaster")
	void AddToasterMessage(const FName& MessageType, const TArray<FText>& Parameters);
};
