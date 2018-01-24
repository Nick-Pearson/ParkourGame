#pragma once


#include "CoreMinimal.h"
#include "CoreUObject.h"

#include "Json.h"

#include "ELowEntryJsonType.h"
#include "ELowEntryJsonValueFound.h"
#include "ELowEntryJsonValueAndTypeFound.h"

#include "LowEntryJsonObject.generated.h"


class ULowEntryJsonObject;
class ULowEntryJsonArray;
class ULowEntryJsonValue;
class ULowEntryJsonObjectEntry;


UCLASS(BlueprintType)
class LOWENTRYJSON_API ULowEntryJsonObject : public UObject
{
	GENERATED_UCLASS_BODY()


public:
	static ULowEntryJsonObject* Create();
	static ULowEntryJsonObject* Create(const TSharedPtr<FJsonObject> Value);
	static ULowEntryJsonObject* Create(const TArray<ULowEntryJsonObjectEntry*>& Value);
	static ULowEntryJsonObject* Clone(ULowEntryJsonObject* JsonObject);
	static ULowEntryJsonObject* Merge(ULowEntryJsonObject* JsonObject1, ULowEntryJsonObject* JsonObject2);
	static TSharedPtr<FJsonObject> CloneData(const TSharedPtr<FJsonObject> Data);


public:
	static FString ToJsonString(ULowEntryJsonObject* JsonObject, const bool PrettyPrint = true);

	static void AddToJson(const TSharedPtr<FJsonObject> Data, const TSharedRef<TJsonWriter<TCHAR, TPrettyJsonPrintPolicy<TCHAR>>> JsonWriter);
	static void AddToJson(const FString& Key, const TSharedPtr<FJsonObject> Data, const TSharedRef<TJsonWriter<TCHAR, TPrettyJsonPrintPolicy<TCHAR>>> JsonWriter);

	static void AddToJson(const TSharedPtr<FJsonObject> Data, const TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter);
	static void AddToJson(const FString& Key, const TSharedPtr<FJsonObject> Data, const TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter);


public:
	TSharedPtr<FJsonObject> Data = NULL;


	static void Clear(ULowEntryJsonObject* JsonObject);
	static void Unset(ULowEntryJsonObject* JsonObject, const FString& Key);
	static bool IsSet(ULowEntryJsonObject* JsonObject, const FString& Key);

	static void SetNull(ULowEntryJsonObject* JsonObject, const FString& Key);
	static void SetString(ULowEntryJsonObject* JsonObject, const FString& Key, const FString& Value);
	static void SetInteger(ULowEntryJsonObject* JsonObject, const FString& Key, const int32 Value);
	static void SetFloat(ULowEntryJsonObject* JsonObject, const FString& Key, const float Value);
	static void SetByte(ULowEntryJsonObject* JsonObject, const FString& Key, const uint8 Value);
	static void SetBoolean(ULowEntryJsonObject* JsonObject, const FString& Key, const bool Value);
	static void SetJsonObject(ULowEntryJsonObject* JsonObject, const FString& Key, ULowEntryJsonObject* Value);
	static void SetJsonArray(ULowEntryJsonObject* JsonObject, const FString& Key, ULowEntryJsonArray* Value);
	static void SetJsonValue(ULowEntryJsonObject* JsonObject, const FString& Key, ULowEntryJsonValue* Value);

	static void Get(ULowEntryJsonObject* JsonObject, const FString& Key, ELowEntryJsonType& Branch, FString& ReturnStringValue, int32& ReturnIntegerValue, float& ReturnFloatValue, uint8& ReturnByteValue, bool& ReturnBooleanValue, ULowEntryJsonObject*& ReturnJsonObjectValue, ULowEntryJsonArray*& ReturnJsonArrayValue);
	static void GetNull(ULowEntryJsonObject* JsonObject, const FString& Key, ELowEntryJsonValueAndTypeFound& Branch);
	static FString GetString(ULowEntryJsonObject* JsonObject, const FString& Key, ELowEntryJsonValueAndTypeFound& Branch);
	static int32 GetInteger(ULowEntryJsonObject* JsonObject, const FString& Key, ELowEntryJsonValueAndTypeFound& Branch);
	static float GetFloat(ULowEntryJsonObject* JsonObject, const FString& Key, ELowEntryJsonValueAndTypeFound& Branch);
	static uint8 GetByte(ULowEntryJsonObject* JsonObject, const FString& Key, ELowEntryJsonValueAndTypeFound& Branch);
	static bool GetBoolean(ULowEntryJsonObject* JsonObject, const FString& Key, ELowEntryJsonValueAndTypeFound& Branch);
	static ULowEntryJsonObject* GetJsonObject(ULowEntryJsonObject* JsonObject, const FString& Key, ELowEntryJsonValueAndTypeFound& Branch);
	static ULowEntryJsonArray* GetJsonArray(ULowEntryJsonObject* JsonObject, const FString& Key, ELowEntryJsonValueAndTypeFound& Branch);
	static ULowEntryJsonValue* GetJsonValue(ULowEntryJsonObject* JsonObject, const FString& Key, ELowEntryJsonValueFound& Branch);

	static FString GetAsStringOrDefault(ULowEntryJsonObject* JsonObject, const FString& Key, const FString& Default);
	static int32 GetAsIntegerOrDefault(ULowEntryJsonObject* JsonObject, const FString& Key, const int32 Default);
	static float GetAsFloatOrDefault(ULowEntryJsonObject* JsonObject, const FString& Key, const float Default);
	static uint8 GetAsByteOrDefault(ULowEntryJsonObject* JsonObject, const FString& Key, const uint8 Default);
	static bool GetAsBooleanOrDefault(ULowEntryJsonObject* JsonObject, const FString& Key, const bool Default);
	static ULowEntryJsonObject* GetAsJsonObjectOrDefault(ULowEntryJsonObject* JsonObject, const FString& Key, ULowEntryJsonObject* Default);
	static ULowEntryJsonArray* GetAsJsonArrayOrDefault(ULowEntryJsonObject* JsonObject, const FString& Key, ULowEntryJsonArray* Default);
	static ULowEntryJsonValue* GetAsJsonValueOrDefault(ULowEntryJsonObject* JsonObject, const FString& Key, ULowEntryJsonValue* Default);

	static TArray<FString> GetKeys(ULowEntryJsonObject* JsonObject);
	static ULowEntryJsonArray* GetValues(ULowEntryJsonObject* JsonObject);
	static int32 Length(ULowEntryJsonObject* JsonObject);

	static void Append(ULowEntryJsonObject* JsonObject, ULowEntryJsonObject* AppendJsonObject, const bool OverrideDuplicates = true);

	static void Sort(ULowEntryJsonObject* JsonObject, const bool Reversed = false);
};
