#pragma once


#include "CoreMinimal.h"
#include "CoreUObject.h"

#include "Json.h"

#include "ELowEntryJsonType.h"
#include "ELowEntryJsonTypeFound.h"

#include "LowEntryJsonObjectEntry.generated.h"


class ULowEntryJsonObject;
class ULowEntryJsonArray;
class ULowEntryJsonValue;


UCLASS(BlueprintType)
class LOWENTRYJSON_API ULowEntryJsonObjectEntry : public UObject
{
	GENERATED_UCLASS_BODY()


public:
	static ULowEntryJsonObjectEntry* Create(const FString& Key, ULowEntryJsonValue* Value);
	static ULowEntryJsonObjectEntry* CreateFromNull(const FString& Key);
	static ULowEntryJsonObjectEntry* CreateFromString(const FString& Key, const FString& Value);
	static ULowEntryJsonObjectEntry* CreateFromInteger(const FString& Key, const int32 Value);
	static ULowEntryJsonObjectEntry* CreateFromFloat(const FString& Key, const float Value);
	static ULowEntryJsonObjectEntry* CreateFromByte(const FString& Key, const uint8 Value);
	static ULowEntryJsonObjectEntry* CreateFromBoolean(const FString& Key, const bool Value);
	static ULowEntryJsonObjectEntry* CreateFromJsonObject(const FString& Key, ULowEntryJsonObject* Value);
	static ULowEntryJsonObjectEntry* CreateFromJsonArray(const FString& Key, ULowEntryJsonArray* Value);


public:
	UPROPERTY()
	FString Key = TEXT("");
	UPROPERTY()
	ULowEntryJsonValue* Value = NULL;
};
