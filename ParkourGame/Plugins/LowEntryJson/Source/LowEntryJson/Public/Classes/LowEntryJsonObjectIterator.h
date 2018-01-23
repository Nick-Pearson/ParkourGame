#pragma once


#include "CoreMinimal.h"
#include "CoreUObject.h"

#include "Json.h"

#include "ELowEntryJsonValueFound.h"

#include "LowEntryJsonObjectIterator.generated.h"


class ULowEntryJsonObject;
class ULowEntryJsonArray;
class ULowEntryJsonValue;


UCLASS(BlueprintType)
class LOWENTRYJSON_API ULowEntryJsonObjectIterator : public UObject
{
	GENERATED_UCLASS_BODY()


public:
	static ULowEntryJsonObjectIterator* Create(ULowEntryJsonObject* JsonObject);
	static ULowEntryJsonObjectIterator* Clone(ULowEntryJsonObjectIterator* JsonObjectIterator);


public:
	UPROPERTY()
	TArray<FString> Keys;
	TArray<TSharedPtr<FJsonValue>> Values;


	static void Get(ULowEntryJsonObjectIterator* JsonObjectIterator, const int32 Index, FString& Key, ULowEntryJsonValue*& JsonValue, ELowEntryJsonValueFound& Branch);


	static TArray<FString> GetKeys(ULowEntryJsonObjectIterator* JsonObjectIterator);

	static ULowEntryJsonArray* GetValues(ULowEntryJsonObjectIterator* JsonObjectIterator);

	static int32 Length(ULowEntryJsonObjectIterator* JsonObjectIterator);
};
