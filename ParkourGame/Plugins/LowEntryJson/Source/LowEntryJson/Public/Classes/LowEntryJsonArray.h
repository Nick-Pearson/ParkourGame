#pragma once


#include "CoreMinimal.h"
#include "CoreUObject.h"

#include "Json.h"

#include "ELowEntryJsonType.h"
#include "ELowEntryJsonValueFound.h"
#include "ELowEntryJsonValueAndTypeFound.h"

#include "LowEntryJsonArray.generated.h"


class ULowEntryJsonObject;
class ULowEntryJsonArray;
class ULowEntryJsonValue;


UCLASS(BlueprintType)
class LOWENTRYJSON_API ULowEntryJsonArray : public UObject
{
	GENERATED_UCLASS_BODY()


public:
	static ULowEntryJsonArray* Create();
	static ULowEntryJsonArray* Create(const TArray<TSharedPtr<FJsonValue>> Value);
	static ULowEntryJsonArray* CreateFromStringArray(const TArray<FString>& Value);
	static ULowEntryJsonArray* CreateFromIntegerArray(const TArray<int32>& Value);
	static ULowEntryJsonArray* CreateFromFloatArray(const TArray<float>& Value);
	static ULowEntryJsonArray* CreateFromByteArray(const TArray<uint8>& Value);
	static ULowEntryJsonArray* CreateFromBooleanArray(const TArray<bool>& Value);
	static ULowEntryJsonArray* CreateFromJsonObjectArray(const TArray<ULowEntryJsonObject*>& Value);
	static ULowEntryJsonArray* CreateFromJsonArrayArray(const TArray<ULowEntryJsonArray*>& Value);
	static ULowEntryJsonArray* CreateFromJsonValueArray(const TArray<ULowEntryJsonValue*>& Value);
	static ULowEntryJsonArray* Clone(ULowEntryJsonArray* JsonArray);
	static ULowEntryJsonArray* Merge(ULowEntryJsonArray* JsonArray1, ULowEntryJsonArray* JsonArray2);


public:
	static FString ToJsonString(ULowEntryJsonArray* JsonArray, const bool PrettyPrint = true);

	static void AddToJson(const TArray<TSharedPtr<FJsonValue>>& Data, const TSharedRef<TJsonWriter<TCHAR, TPrettyJsonPrintPolicy<TCHAR>>> JsonWriter);
	static void AddToJson(const FString& Key, const TArray<TSharedPtr<FJsonValue>>& Data, const TSharedRef<TJsonWriter<TCHAR, TPrettyJsonPrintPolicy<TCHAR>>> JsonWriter);

	static void AddToJson(const TArray<TSharedPtr<FJsonValue>>& Data, const TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter);
	static void AddToJson(const FString& Key, const TArray<TSharedPtr<FJsonValue>>& Data, const TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter);


public:
	TArray<TSharedPtr<FJsonValue>> Data;


	static void Clear(ULowEntryJsonArray* JsonArray);
	static void Unset(ULowEntryJsonArray* JsonArray, const int32 Index, const int32 Count = 1);
	static bool IsSet(ULowEntryJsonArray* JsonArray, const int32 Index);

	static void AddNull(ULowEntryJsonArray* JsonArray);
	static void AddString(ULowEntryJsonArray* JsonArray, const FString& Value);
	static void AddInteger(ULowEntryJsonArray* JsonArray, const int32 Value);
	static void AddFloat(ULowEntryJsonArray* JsonArray, const float Value);
	static void AddByte(ULowEntryJsonArray* JsonArray, const uint8 Value);
	static void AddBoolean(ULowEntryJsonArray* JsonArray, const bool Value);
	static void AddJsonObject(ULowEntryJsonArray* JsonArray, ULowEntryJsonObject* Value);
	static void AddJsonArray(ULowEntryJsonArray* JsonArray, ULowEntryJsonArray* Value);
	static void AddJsonValue(ULowEntryJsonArray* JsonArray, ULowEntryJsonValue* Value);

	static void SetNull(ULowEntryJsonArray* JsonArray, const int32 Index);
	static void SetString(ULowEntryJsonArray* JsonArray, const int32 Index, const FString& Value);
	static void SetInteger(ULowEntryJsonArray* JsonArray, const int32 Index, const int32 Value);
	static void SetFloat(ULowEntryJsonArray* JsonArray, const int32 Index, const float Value);
	static void SetByte(ULowEntryJsonArray* JsonArray, const int32 Index, const uint8 Value);
	static void SetBoolean(ULowEntryJsonArray* JsonArray, const int32 Index, const bool Value);
	static void SetJsonObject(ULowEntryJsonArray* JsonArray, const int32 Index, ULowEntryJsonObject* Value);
	static void SetJsonArray(ULowEntryJsonArray* JsonArray, const int32 Index, ULowEntryJsonArray* Value);
	static void SetJsonValue(ULowEntryJsonArray* JsonArray, const int32 Index, ULowEntryJsonValue* Value);

	static void InsertNull(ULowEntryJsonArray* JsonArray, const int32 Index);
	static void InsertString(ULowEntryJsonArray* JsonArray, const int32 Index, const FString& Value);
	static void InsertInteger(ULowEntryJsonArray* JsonArray, const int32 Index, const int32 Value);
	static void InsertFloat(ULowEntryJsonArray* JsonArray, const int32 Index, const float Value);
	static void InsertByte(ULowEntryJsonArray* JsonArray, const int32 Index, const uint8 Value);
	static void InsertBoolean(ULowEntryJsonArray* JsonArray, const int32 Index, const bool Value);
	static void InsertJsonObject(ULowEntryJsonArray* JsonArray, const int32 Index, ULowEntryJsonObject* Value);
	static void InsertJsonArray(ULowEntryJsonArray* JsonArray, const int32 Index, ULowEntryJsonArray* Value);
	static void InsertJsonValue(ULowEntryJsonArray* JsonArray, const int32 Index, ULowEntryJsonValue* Value);

	static void Get(ULowEntryJsonArray* JsonArray, const int32 Index, ELowEntryJsonType& Branch, FString& ReturnStringValue, int32& ReturnIntegerValue, float& ReturnFloatValue, uint8& ReturnByteValue, bool& ReturnBooleanValue, ULowEntryJsonObject*& ReturnJsonObjectValue, ULowEntryJsonArray*& ReturnJsonArrayValue);
	static void GetNull(ULowEntryJsonArray* JsonArray, const int32 Index, ELowEntryJsonValueAndTypeFound& Branch);
	static FString GetString(ULowEntryJsonArray* JsonArray, const int32 Index, ELowEntryJsonValueAndTypeFound& Branch);
	static int32 GetInteger(ULowEntryJsonArray* JsonArray, const int32 Index, ELowEntryJsonValueAndTypeFound& Branch);
	static float GetFloat(ULowEntryJsonArray* JsonArray, const int32 Index, ELowEntryJsonValueAndTypeFound& Branch);
	static uint8 GetByte(ULowEntryJsonArray* JsonArray, const int32 Index, ELowEntryJsonValueAndTypeFound& Branch);
	static bool GetBoolean(ULowEntryJsonArray* JsonArray, const int32 Index, ELowEntryJsonValueAndTypeFound& Branch);
	static ULowEntryJsonObject* GetJsonObject(ULowEntryJsonArray* JsonArray, const int32 Index, ELowEntryJsonValueAndTypeFound& Branch);
	static ULowEntryJsonArray* GetJsonArray(ULowEntryJsonArray* JsonArray, const int32 Index, ELowEntryJsonValueAndTypeFound& Branch);
	static ULowEntryJsonValue* GetJsonValue(ULowEntryJsonArray* JsonArray, const int32 Index, ELowEntryJsonValueFound& Branch);

	static FString GetAsStringOrDefault(ULowEntryJsonArray* JsonArray, const int32 Index, const FString& Default);
	static int32 GetAsIntegerOrDefault(ULowEntryJsonArray* JsonArray, const int32 Index, const int32 Default);
	static float GetAsFloatOrDefault(ULowEntryJsonArray* JsonArray, const int32 Index, const float Default);
	static uint8 GetAsByteOrDefault(ULowEntryJsonArray* JsonArray, const int32 Index, const uint8 Default);
	static bool GetAsBooleanOrDefault(ULowEntryJsonArray* JsonArray, const int32 Index, const bool Default);
	static ULowEntryJsonObject* GetAsJsonObjectOrDefault(ULowEntryJsonArray* JsonArray, const int32 Index, ULowEntryJsonObject* Default);
	static ULowEntryJsonArray* GetAsJsonArrayOrDefault(ULowEntryJsonArray* JsonArray, const int32 Index, ULowEntryJsonArray* Default);
	static ULowEntryJsonValue* GetAsJsonValueOrDefault(ULowEntryJsonArray* JsonArray, const int32 Index, ULowEntryJsonValue* Default);

	static int32 Length(ULowEntryJsonArray* JsonArray);

	static void Resize(ULowEntryJsonArray* JsonArray, const int32 Size);
	static void Expand(ULowEntryJsonArray* JsonArray, const int32 Size);
	static void Shorten(ULowEntryJsonArray* JsonArray, const int32 Size);

	static void Append(ULowEntryJsonArray* JsonArray, ULowEntryJsonArray* AppendJsonArray);
};
