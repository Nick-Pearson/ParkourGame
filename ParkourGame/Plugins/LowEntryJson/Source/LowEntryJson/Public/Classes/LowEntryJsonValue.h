#pragma once


#include "CoreMinimal.h"
#include "CoreUObject.h"

#include "Json.h"

#include "ELowEntryJsonType.h"
#include "ELowEntryJsonTypeFound.h"

#include "LowEntryJsonValue.generated.h"


class ULowEntryJsonObject;
class ULowEntryJsonArray;
class ULowEntryJsonValue;


UCLASS(BlueprintType)
class LOWENTRYJSON_API ULowEntryJsonValue : public UObject
{
	GENERATED_UCLASS_BODY()


public:
	static ULowEntryJsonValue* Create();
	static ULowEntryJsonValue* Create(const TSharedPtr<FJsonValue> Value);
	static ULowEntryJsonValue* CreateFromNull();
	static ULowEntryJsonValue* CreateFromString(const FString& Value);
	static ULowEntryJsonValue* CreateFromInteger(const int32 Value);
	static ULowEntryJsonValue* CreateFromFloat(const float Value);
	static ULowEntryJsonValue* CreateFromByte(const uint8 Value);
	static ULowEntryJsonValue* CreateFromBoolean(const bool Value);
	static ULowEntryJsonValue* CreateFromJsonObject(ULowEntryJsonObject* Value);
	static ULowEntryJsonValue* CreateFromJsonArray(ULowEntryJsonArray* Value);
	static ULowEntryJsonValue* Clone(ULowEntryJsonValue* JsonValue);
	static TSharedPtr<FJsonValue> CloneData(const TSharedPtr<FJsonValue> Data);


public:
	static void AddToJson(const TSharedPtr<FJsonValue> Data, const TSharedRef<TJsonWriter<TCHAR, TPrettyJsonPrintPolicy<TCHAR>>> JsonWriter);
	static void AddToJson(const FString& Key, const TSharedPtr<FJsonValue> Data, const TSharedRef<TJsonWriter<TCHAR, TPrettyJsonPrintPolicy<TCHAR>>> JsonWriter);

	static void AddToJson(const TSharedPtr<FJsonValue> Data, const TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter);
	static void AddToJson(const FString& Key, const TSharedPtr<FJsonValue> Data, const TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter);


public:
	TSharedPtr<FJsonValue> Data = NULL;


	static void Set(ULowEntryJsonValue* JsonValue, ULowEntryJsonValue* Value);
	static void SetNull(ULowEntryJsonValue* JsonValue);
	static void SetString(ULowEntryJsonValue* JsonValue, const FString& Value);
	static void SetInteger(ULowEntryJsonValue* JsonValue, const int32 Value);
	static void SetFloat(ULowEntryJsonValue* JsonValue, const float Value);
	static void SetByte(ULowEntryJsonValue* JsonValue, const uint8 Value);
	static void SetBoolean(ULowEntryJsonValue* JsonValue, const bool Value);
	static void SetJsonObject(ULowEntryJsonValue* JsonValue, ULowEntryJsonObject* Value);
	static void SetJsonArray(ULowEntryJsonValue* JsonValue, ULowEntryJsonArray* Value);

	static void Get(ULowEntryJsonValue* JsonValue, ELowEntryJsonType& Branch, FString& ReturnStringValue, int32& ReturnIntegerValue, float& ReturnFloatValue, uint8& ReturnByteValue, bool& ReturnBooleanValue, ULowEntryJsonObject*& ReturnJsonObjectValue, ULowEntryJsonArray*& ReturnJsonArrayValue);
	static void GetNull(ULowEntryJsonValue* JsonValue, ELowEntryJsonTypeFound& Branch);
	static FString GetString(ULowEntryJsonValue* JsonValue, ELowEntryJsonTypeFound& Branch);
	static int32 GetInteger(ULowEntryJsonValue* JsonValue, ELowEntryJsonTypeFound& Branch);
	static float GetFloat(ULowEntryJsonValue* JsonValue, ELowEntryJsonTypeFound& Branch);
	static uint8 GetByte(ULowEntryJsonValue* JsonValue, ELowEntryJsonTypeFound& Branch);
	static bool GetBoolean(ULowEntryJsonValue* JsonValue, ELowEntryJsonTypeFound& Branch);
	static ULowEntryJsonObject* GetJsonObject(ULowEntryJsonValue* JsonValue, ELowEntryJsonTypeFound& Branch);
	static ULowEntryJsonArray* GetJsonArray(ULowEntryJsonValue* JsonValue, ELowEntryJsonTypeFound& Branch);

	static FString GetAsStringOrDefault(ULowEntryJsonValue* JsonValue, const FString& Default);
	static int32 GetAsIntegerOrDefault(ULowEntryJsonValue* JsonValue, const int32 Default);
	static float GetAsFloatOrDefault(ULowEntryJsonValue* JsonValue, const float Default);
	static uint8 GetAsByteOrDefault(ULowEntryJsonValue* JsonValue, const uint8 Default);
	static bool GetAsBooleanOrDefault(ULowEntryJsonValue* JsonValue, const bool Default);
	static ULowEntryJsonObject* GetAsJsonObjectOrDefault(ULowEntryJsonValue* JsonValue, ULowEntryJsonObject* Default);
	static ULowEntryJsonArray* GetAsJsonArrayOrDefault(ULowEntryJsonValue* JsonValue, ULowEntryJsonArray* Default);
};
