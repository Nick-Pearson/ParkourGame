#include "LowEntryJsonLibrary.h"
#include "LowEntryJsonObject.h"
#include "LowEntryJsonArray.h"
#include "LowEntryJsonValue.h"
#include "LowEntryJsonObjectIterator.h"
#include "LowEntryJsonObjectEntry.h"



// init >>
	ULowEntryJsonLibrary::ULowEntryJsonLibrary(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
	{
	}
// init <<



// json >>
	void ULowEntryJsonLibrary::Json_ParseJsonString(const FString& JsonString, ULowEntryJsonObject*& ReturnJsonObjectValue, ULowEntryJsonArray*& ReturnJsonArrayValue, ELowEntryJsonParseResult& Branch)
	{
		FString TrimmedJsonString = JsonString;
		TrimmedJsonString.TrimStartAndEndInline();
		if(TrimmedJsonString.Len() <= 0)
		{
			Branch = ELowEntryJsonParseResult::StringIsEmpty;
			return;
		}

		if(TrimmedJsonString.StartsWith(TEXT("{")) && TrimmedJsonString.EndsWith(TEXT("}")))
		{
			TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(TrimmedJsonString);
			TSharedPtr<FJsonObject> JsonObjectData;
			if(FJsonSerializer::Deserialize(JsonReader, JsonObjectData))
			{
				ReturnJsonObjectValue = ULowEntryJsonObject::Create(JsonObjectData);
				Branch = ELowEntryJsonParseResult::FoundObject;
				return;
			}
		}

		// Parsing Array doesn't work, hotfix is underneath
		//if(TrimmedJsonString.StartsWith(TEXT("[")) && TrimmedJsonString.EndsWith(TEXT("]")))
		//{
		//	TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(TrimmedJsonString);
		//	TArray<TSharedPtr<FJsonValue>> JsonArrayData;
		//	if(FJsonSerializer::Deserialize(JsonReader, JsonArrayData))
		//	{
		//		ReturnJsonArrayValue = ULowEntryJsonArray::Create(JsonArrayData);
		//		Branch = ELowEntryJsonParseResult::FoundArray;
		//		return;
		//	}
		//}

		if(TrimmedJsonString.StartsWith(TEXT("[")) && TrimmedJsonString.EndsWith(TEXT("]")))
		{
			// Encapsulates the Array in an Object, so it can be parsed
			FString ObjectTrimmedJsonString = TEXT("{\"array\":");
			ObjectTrimmedJsonString += TrimmedJsonString;
			ObjectTrimmedJsonString += TEXT("}");

			TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(ObjectTrimmedJsonString);
			TSharedPtr<FJsonObject> JsonObjectData;
			if(FJsonSerializer::Deserialize(JsonReader, JsonObjectData))
			{
				const TArray<TSharedPtr<FJsonValue>>* JsonArrayData;
				if(JsonObjectData->TryGetArrayField(TEXT("array"), JsonArrayData))
				{
					ReturnJsonArrayValue = ULowEntryJsonArray::Create(*JsonArrayData);
					Branch = ELowEntryJsonParseResult::FoundArray;
					return;
				}
			}
		}

		Branch = ELowEntryJsonParseResult::UnableToParse;
	}
// json <<



// object >>
	ULowEntryJsonObject* ULowEntryJsonLibrary::Object_Create()
	{
		return ULowEntryJsonObject::Create();
	}

	ULowEntryJsonObject* ULowEntryJsonLibrary::Object_CreateFromObjectEntryArray(const TArray<ULowEntryJsonObjectEntry*>& Array)
	{
		return ULowEntryJsonObject::Create(Array);
	}

	ULowEntryJsonObject* ULowEntryJsonLibrary::Object_Clone(ULowEntryJsonObject* JsonObject)
	{
		return ULowEntryJsonObject::Clone(JsonObject);
	}

	ULowEntryJsonObject* ULowEntryJsonLibrary::Object_Merge(ULowEntryJsonObject* JsonObject1, ULowEntryJsonObject* JsonObject2)
	{
		return ULowEntryJsonObject::Merge(JsonObject1, JsonObject2);
	}


	ULowEntryJsonObject* ULowEntryJsonLibrary::Object_Pure_Create()
	{
		return ULowEntryJsonObject::Create();
	}

	ULowEntryJsonObject* ULowEntryJsonLibrary::Object_Pure_CreateFromObjectEntryArray(const TArray<ULowEntryJsonObjectEntry*>& Array)
	{
		return ULowEntryJsonObject::Create(Array);
	}

	ULowEntryJsonObject* ULowEntryJsonLibrary::Object_Pure_Clone(ULowEntryJsonObject* JsonObject)
	{
		return ULowEntryJsonObject::Clone(JsonObject);
	}

	ULowEntryJsonObject* ULowEntryJsonLibrary::Object_Pure_Merge(ULowEntryJsonObject* JsonObject1, ULowEntryJsonObject* JsonObject2)
	{
		return ULowEntryJsonObject::Merge(JsonObject1, JsonObject2);
	}


	FString ULowEntryJsonLibrary::Object_ToJsonString(ULowEntryJsonObject* JsonObject, const bool PrettyPrint)
	{
		return ULowEntryJsonObject::ToJsonString(JsonObject, PrettyPrint);
	}

	FString ULowEntryJsonLibrary::Object_Pure_ToJsonString(ULowEntryJsonObject* JsonObject, const bool PrettyPrint)
	{
		return ULowEntryJsonObject::ToJsonString(JsonObject, PrettyPrint);
	}


	void ULowEntryJsonLibrary::Object_Clear(ULowEntryJsonObject* JsonObject)
	{
		ULowEntryJsonObject::Clear(JsonObject);
	}

	void ULowEntryJsonLibrary::Object_Unset(ULowEntryJsonObject* JsonObject, const FString& Key)
	{
		ULowEntryJsonObject::Unset(JsonObject, Key);
	}

	bool ULowEntryJsonLibrary::Object_IsSet(ULowEntryJsonObject* JsonObject, const FString& Key)
	{
		return ULowEntryJsonObject::IsSet(JsonObject, Key);
	}


	void ULowEntryJsonLibrary::Object_SetNull(ULowEntryJsonObject* JsonObject, const FString& Key)
	{
		ULowEntryJsonObject::SetNull(JsonObject, Key);
	}

	void ULowEntryJsonLibrary::Object_SetString(ULowEntryJsonObject* JsonObject, const FString& Key, const FString& Value)
	{
		ULowEntryJsonObject::SetString(JsonObject, Key, Value);
	}

	void ULowEntryJsonLibrary::Object_SetInteger(ULowEntryJsonObject* JsonObject, const FString& Key, const int32 Value)
	{
		ULowEntryJsonObject::SetInteger(JsonObject, Key, Value);
	}

	void ULowEntryJsonLibrary::Object_SetFloat(ULowEntryJsonObject* JsonObject, const FString& Key, const float Value)
	{
		ULowEntryJsonObject::SetFloat(JsonObject, Key, Value);
	}

	void ULowEntryJsonLibrary::Object_SetByte(ULowEntryJsonObject* JsonObject, const FString& Key, const uint8 Value)
	{
		ULowEntryJsonObject::SetByte(JsonObject, Key, Value);
	}

	void ULowEntryJsonLibrary::Object_SetBoolean(ULowEntryJsonObject* JsonObject, const FString& Key, const bool Value)
	{
		ULowEntryJsonObject::SetBoolean(JsonObject, Key, Value);
	}

	void ULowEntryJsonLibrary::Object_SetJsonObject(ULowEntryJsonObject* JsonObject, const FString& Key, ULowEntryJsonObject* Value)
	{
		ULowEntryJsonObject::SetJsonObject(JsonObject, Key, Value);
	}

	void ULowEntryJsonLibrary::Object_SetJsonArray(ULowEntryJsonObject* JsonObject, const FString& Key, ULowEntryJsonArray* Value)
	{
		ULowEntryJsonObject::SetJsonArray(JsonObject, Key, Value);
	}

	void ULowEntryJsonLibrary::Object_SetJsonValue(ULowEntryJsonObject* JsonObject, const FString& Key, ULowEntryJsonValue* Value)
	{
		ULowEntryJsonObject::SetJsonValue(JsonObject, Key, Value);
	}


	void ULowEntryJsonLibrary::Object_Get(ULowEntryJsonObject* JsonObject, const FString& Key, ELowEntryJsonType& Branch, FString& ReturnStringValue, int32& ReturnIntegerValue, float& ReturnFloatValue, uint8& ReturnByteValue, bool& ReturnBooleanValue, ULowEntryJsonObject*& ReturnJsonObjectValue, ULowEntryJsonArray*& ReturnJsonArrayValue)
	{
		ULowEntryJsonObject::Get(JsonObject, Key, Branch, ReturnStringValue, ReturnIntegerValue, ReturnFloatValue, ReturnByteValue, ReturnBooleanValue, ReturnJsonObjectValue, ReturnJsonArrayValue);
	}

	void ULowEntryJsonLibrary::Object_GetNull(ULowEntryJsonObject* JsonObject, const FString& Key, ELowEntryJsonValueAndTypeFound& Branch)
	{
		ULowEntryJsonObject::GetNull(JsonObject, Key, Branch);
	}

	FString ULowEntryJsonLibrary::Object_GetString(ULowEntryJsonObject* JsonObject, const FString& Key, ELowEntryJsonValueAndTypeFound& Branch)
	{
		return ULowEntryJsonObject::GetString(JsonObject, Key, Branch);
	}

	int32 ULowEntryJsonLibrary::Object_GetInteger(ULowEntryJsonObject* JsonObject, const FString& Key, ELowEntryJsonValueAndTypeFound& Branch)
	{
		return ULowEntryJsonObject::GetInteger(JsonObject, Key, Branch);
	}

	float ULowEntryJsonLibrary::Object_GetFloat(ULowEntryJsonObject* JsonObject, const FString& Key, ELowEntryJsonValueAndTypeFound& Branch)
	{
		return ULowEntryJsonObject::GetFloat(JsonObject, Key, Branch);
	}

	uint8 ULowEntryJsonLibrary::Object_GetByte(ULowEntryJsonObject* JsonObject, const FString& Key, ELowEntryJsonValueAndTypeFound& Branch)
	{
		return ULowEntryJsonObject::GetByte(JsonObject, Key, Branch);
	}

	bool ULowEntryJsonLibrary::Object_GetBoolean(ULowEntryJsonObject* JsonObject, const FString& Key, ELowEntryJsonValueAndTypeFound& Branch)
	{
		return ULowEntryJsonObject::GetBoolean(JsonObject, Key, Branch);
	}

	ULowEntryJsonObject* ULowEntryJsonLibrary::Object_GetJsonObject(ULowEntryJsonObject* JsonObject, const FString& Key, ELowEntryJsonValueAndTypeFound& Branch)
	{
		return ULowEntryJsonObject::GetJsonObject(JsonObject, Key, Branch);
	}

	ULowEntryJsonArray* ULowEntryJsonLibrary::Object_GetJsonArray(ULowEntryJsonObject* JsonObject, const FString& Key, ELowEntryJsonValueAndTypeFound& Branch)
	{
		return ULowEntryJsonObject::GetJsonArray(JsonObject, Key, Branch);
	}

	ULowEntryJsonValue* ULowEntryJsonLibrary::Object_GetJsonValue(ULowEntryJsonObject* JsonObject, const FString& Key, ELowEntryJsonValueFound& Branch)
	{
		return ULowEntryJsonObject::GetJsonValue(JsonObject, Key, Branch);
	}


	FString ULowEntryJsonLibrary::Object_GetAsStringOrDefault(ULowEntryJsonObject* JsonObject, const FString& Key, const FString& Default)
	{
		return ULowEntryJsonObject::GetAsStringOrDefault(JsonObject, Key, Default);
	}

	int32 ULowEntryJsonLibrary::Object_GetAsIntegerOrDefault(ULowEntryJsonObject* JsonObject, const FString& Key, const int32 Default)
	{
		return ULowEntryJsonObject::GetAsIntegerOrDefault(JsonObject, Key, Default);
	}

	float ULowEntryJsonLibrary::Object_GetAsFloatOrDefault(ULowEntryJsonObject* JsonObject, const FString& Key, const float Default)
	{
		return ULowEntryJsonObject::GetAsFloatOrDefault(JsonObject, Key, Default);
	}

	uint8 ULowEntryJsonLibrary::Object_GetAsByteOrDefault(ULowEntryJsonObject* JsonObject, const FString& Key, const uint8 Default)
	{
		return ULowEntryJsonObject::GetAsByteOrDefault(JsonObject, Key, Default);
	}

	bool ULowEntryJsonLibrary::Object_GetAsBooleanOrDefault(ULowEntryJsonObject* JsonObject, const FString& Key, const bool Default)
	{
		return ULowEntryJsonObject::GetAsBooleanOrDefault(JsonObject, Key, Default);
	}

	ULowEntryJsonObject* ULowEntryJsonLibrary::Object_GetAsJsonObjectOrDefault(ULowEntryJsonObject* JsonObject, const FString& Key, ULowEntryJsonObject* Default)
	{
		return ULowEntryJsonObject::GetAsJsonObjectOrDefault(JsonObject, Key, Default);
	}

	ULowEntryJsonArray* ULowEntryJsonLibrary::Object_GetAsJsonArrayOrDefault(ULowEntryJsonObject* JsonObject, const FString& Key, ULowEntryJsonArray* Default)
	{
		return ULowEntryJsonObject::GetAsJsonArrayOrDefault(JsonObject, Key, Default);
	}

	ULowEntryJsonValue* ULowEntryJsonLibrary::Object_GetAsJsonValueOrDefault(ULowEntryJsonObject* JsonObject, const FString& Key, ULowEntryJsonValue* Default)
	{
		return ULowEntryJsonObject::GetAsJsonValueOrDefault(JsonObject, Key, Default);
	}


	TArray<FString> ULowEntryJsonLibrary::Object_GetKeys(ULowEntryJsonObject* JsonObject)
	{
		return ULowEntryJsonObject::GetKeys(JsonObject);
	}

	ULowEntryJsonArray* ULowEntryJsonLibrary::Object_GetValues(ULowEntryJsonObject* JsonObject)
	{
		return ULowEntryJsonObject::GetValues(JsonObject);
	}

	int32 ULowEntryJsonLibrary::Object_Length(ULowEntryJsonObject* JsonObject)
	{
		return ULowEntryJsonObject::Length(JsonObject);
	}


	void ULowEntryJsonLibrary::Object_Append(ULowEntryJsonObject* JsonObject, ULowEntryJsonObject* AppendJsonObject, const bool OverrideDuplicates)
	{
		return ULowEntryJsonObject::Append(JsonObject, AppendJsonObject, OverrideDuplicates);
	}


	void ULowEntryJsonLibrary::Object_Sort(ULowEntryJsonObject* JsonObject, const bool Reversed)
	{
		return ULowEntryJsonObject::Sort(JsonObject, Reversed);
	}
// object <<



// object entry >>
	ULowEntryJsonObjectEntry* ULowEntryJsonLibrary::ObjectEntry_Pure_Create(const FString& Key, ULowEntryJsonValue* Value)
	{
		return ULowEntryJsonObjectEntry::Create(Key, Value);
	}

	ULowEntryJsonObjectEntry* ULowEntryJsonLibrary::ObjectEntry_Pure_CreateFromNull(const FString& Key)
	{
		return ULowEntryJsonObjectEntry::CreateFromNull(Key);
	}

	ULowEntryJsonObjectEntry* ULowEntryJsonLibrary::ObjectEntry_Pure_CreateFromString(const FString& Key, const FString& Value)
	{
		return ULowEntryJsonObjectEntry::CreateFromString(Key, Value);
	}

	ULowEntryJsonObjectEntry* ULowEntryJsonLibrary::ObjectEntry_Pure_CreateFromInteger(const FString& Key, const int32 Value)
	{
		return ULowEntryJsonObjectEntry::CreateFromInteger(Key, Value);
	}

	ULowEntryJsonObjectEntry* ULowEntryJsonLibrary::ObjectEntry_Pure_CreateFromFloat(const FString& Key, const float Value)
	{
		return ULowEntryJsonObjectEntry::CreateFromFloat(Key, Value);
	}

	ULowEntryJsonObjectEntry* ULowEntryJsonLibrary::ObjectEntry_Pure_CreateFromByte(const FString& Key, const uint8 Value)
	{
		return ULowEntryJsonObjectEntry::CreateFromByte(Key, Value);
	}

	ULowEntryJsonObjectEntry* ULowEntryJsonLibrary::ObjectEntry_Pure_CreateFromBoolean(const FString& Key, const bool Value)
	{
		return ULowEntryJsonObjectEntry::CreateFromBoolean(Key, Value);
	}

	ULowEntryJsonObjectEntry* ULowEntryJsonLibrary::ObjectEntry_Pure_CreateFromJsonObject(const FString& Key, ULowEntryJsonObject* Value)
	{
		return ULowEntryJsonObjectEntry::CreateFromJsonObject(Key, Value);
	}

	ULowEntryJsonObjectEntry* ULowEntryJsonLibrary::ObjectEntry_Pure_CreateFromJsonArray(const FString& Key, ULowEntryJsonArray* Value)
	{
		return ULowEntryJsonObjectEntry::CreateFromJsonArray(Key, Value);
	}
// object entry <<



// object iterator >>
	ULowEntryJsonObjectIterator* ULowEntryJsonLibrary::ObjectIterator_Create(ULowEntryJsonObject* JsonObject)
	{
		return ULowEntryJsonObjectIterator::Create(JsonObject);
	}

	ULowEntryJsonObjectIterator* ULowEntryJsonLibrary::ObjectIterator_Clone(ULowEntryJsonObjectIterator* JsonObjectIterator)
	{
		return ULowEntryJsonObjectIterator::Clone(JsonObjectIterator);
	}


	void ULowEntryJsonLibrary::ObjectIterator_Get(ULowEntryJsonObjectIterator* JsonObjectIterator, const int32 Index, FString& Key, ULowEntryJsonValue*& JsonValue, ELowEntryJsonValueFound& Branch)
	{
		ULowEntryJsonObjectIterator::Get(JsonObjectIterator, Index, Key, JsonValue, Branch);
	}


	TArray<FString> ULowEntryJsonLibrary::ObjectIterator_GetKeys(ULowEntryJsonObjectIterator* JsonObjectIterator)
	{
		return ULowEntryJsonObjectIterator::GetKeys(JsonObjectIterator);
	}

	ULowEntryJsonArray* ULowEntryJsonLibrary::ObjectIterator_GetValues(ULowEntryJsonObjectIterator* JsonObjectIterator)
	{
		return ULowEntryJsonObjectIterator::GetValues(JsonObjectIterator);
	}

	int32 ULowEntryJsonLibrary::ObjectIterator_Length(ULowEntryJsonObjectIterator* JsonObjectIterator)
	{
		return ULowEntryJsonObjectIterator::Length(JsonObjectIterator);
	}
// object iterator <<



// array >>
	ULowEntryJsonArray* ULowEntryJsonLibrary::Array_Create()
	{
		return ULowEntryJsonArray::Create();
	}

	ULowEntryJsonArray* ULowEntryJsonLibrary::Array_CreateFromStringArray(const TArray<FString>& Value)
	{
		return ULowEntryJsonArray::CreateFromStringArray(Value);
	}

	ULowEntryJsonArray* ULowEntryJsonLibrary::Array_CreateFromIntegerArray(const TArray<int32>& Value)
	{
		return ULowEntryJsonArray::CreateFromIntegerArray(Value);
	}

	ULowEntryJsonArray* ULowEntryJsonLibrary::Array_CreateFromFloatArray(const TArray<float>& Value)
	{
		return ULowEntryJsonArray::CreateFromFloatArray(Value);
	}

	ULowEntryJsonArray* ULowEntryJsonLibrary::Array_CreateFromByteArray(const TArray<uint8>& Value)
	{
		return ULowEntryJsonArray::CreateFromByteArray(Value);
	}

	ULowEntryJsonArray* ULowEntryJsonLibrary::Array_CreateFromBooleanArray(const TArray<bool>& Value)
	{
		return ULowEntryJsonArray::CreateFromBooleanArray(Value);
	}

	ULowEntryJsonArray* ULowEntryJsonLibrary::Array_CreateFromJsonObjectArray(const TArray<ULowEntryJsonObject*>& Value)
	{
		return ULowEntryJsonArray::CreateFromJsonObjectArray(Value);
	}

	ULowEntryJsonArray* ULowEntryJsonLibrary::Array_CreateFromJsonArrayArray(const TArray<ULowEntryJsonArray*>& Value)
	{
		return ULowEntryJsonArray::CreateFromJsonArrayArray(Value);
	}

	ULowEntryJsonArray* ULowEntryJsonLibrary::Array_CreateFromJsonValueArray(const TArray<ULowEntryJsonValue*>& Value)
	{
		return ULowEntryJsonArray::CreateFromJsonValueArray(Value);
	}

	ULowEntryJsonArray* ULowEntryJsonLibrary::Array_Clone(ULowEntryJsonArray* JsonArray)
	{
		return ULowEntryJsonArray::Clone(JsonArray);
	}

	ULowEntryJsonArray* ULowEntryJsonLibrary::Array_Merge(ULowEntryJsonArray* JsonArray1, ULowEntryJsonArray* JsonArray2)
	{
		return ULowEntryJsonArray::Merge(JsonArray1, JsonArray2);
	}


	ULowEntryJsonArray* ULowEntryJsonLibrary::Array_Pure_Create()
	{
		return ULowEntryJsonArray::Create();
	}

	ULowEntryJsonArray* ULowEntryJsonLibrary::Array_Pure_CreateFromStringArray(const TArray<FString>& Value)
	{
		return ULowEntryJsonArray::CreateFromStringArray(Value);
	}

	ULowEntryJsonArray* ULowEntryJsonLibrary::Array_Pure_CreateFromIntegerArray(const TArray<int32>& Value)
	{
		return ULowEntryJsonArray::CreateFromIntegerArray(Value);
	}

	ULowEntryJsonArray* ULowEntryJsonLibrary::Array_Pure_CreateFromFloatArray(const TArray<float>& Value)
	{
		return ULowEntryJsonArray::CreateFromFloatArray(Value);
	}

	ULowEntryJsonArray* ULowEntryJsonLibrary::Array_Pure_CreateFromByteArray(const TArray<uint8>& Value)
	{
		return ULowEntryJsonArray::CreateFromByteArray(Value);
	}

	ULowEntryJsonArray* ULowEntryJsonLibrary::Array_Pure_CreateFromBooleanArray(const TArray<bool>& Value)
	{
		return ULowEntryJsonArray::CreateFromBooleanArray(Value);
	}

	ULowEntryJsonArray* ULowEntryJsonLibrary::Array_Pure_CreateFromJsonObjectArray(const TArray<ULowEntryJsonObject*>& Value)
	{
		return ULowEntryJsonArray::CreateFromJsonObjectArray(Value);
	}

	ULowEntryJsonArray* ULowEntryJsonLibrary::Array_Pure_CreateFromJsonArrayArray(const TArray<ULowEntryJsonArray*>& Value)
	{
		return ULowEntryJsonArray::CreateFromJsonArrayArray(Value);
	}

	ULowEntryJsonArray* ULowEntryJsonLibrary::Array_Pure_CreateFromJsonValueArray(const TArray<ULowEntryJsonValue*>& Value)
	{
		return ULowEntryJsonArray::CreateFromJsonValueArray(Value);
	}

	ULowEntryJsonArray* ULowEntryJsonLibrary::Array_Pure_Clone(ULowEntryJsonArray* JsonArray)
	{
		return ULowEntryJsonArray::Clone(JsonArray);
	}

	ULowEntryJsonArray* ULowEntryJsonLibrary::Array_Pure_Merge(ULowEntryJsonArray* JsonArray1, ULowEntryJsonArray* JsonArray2)
	{
		return ULowEntryJsonArray::Merge(JsonArray1, JsonArray2);
	}


	FString ULowEntryJsonLibrary::Array_ToJsonString(ULowEntryJsonArray* JsonArray, const bool PrettyPrint)
	{
		return ULowEntryJsonArray::ToJsonString(JsonArray, PrettyPrint);
	}

	FString ULowEntryJsonLibrary::Array_Pure_ToJsonString(ULowEntryJsonArray* JsonArray, const bool PrettyPrint)
	{
		return ULowEntryJsonArray::ToJsonString(JsonArray, PrettyPrint);
	}


	void ULowEntryJsonLibrary::Array_Clear(ULowEntryJsonArray* JsonArray)
	{
		ULowEntryJsonArray::Clear(JsonArray);
	}

	void ULowEntryJsonLibrary::Array_Unset(ULowEntryJsonArray* JsonArray, const int32 Index, const int32 Count)
	{
		ULowEntryJsonArray::Unset(JsonArray, Index, Count);
	}

	bool ULowEntryJsonLibrary::Array_IsSet(ULowEntryJsonArray* JsonArray, const int32 Index)
	{
		return ULowEntryJsonArray::IsSet(JsonArray, Index);
	}


	void ULowEntryJsonLibrary::Array_AddNull(ULowEntryJsonArray* JsonArray)
	{
		ULowEntryJsonArray::AddNull(JsonArray);
	}

	void ULowEntryJsonLibrary::Array_AddString(ULowEntryJsonArray* JsonArray, const FString& Value)
	{
		ULowEntryJsonArray::AddString(JsonArray, Value);
	}

	void ULowEntryJsonLibrary::Array_AddInteger(ULowEntryJsonArray* JsonArray, const int32 Value)
	{
		ULowEntryJsonArray::AddInteger(JsonArray, Value);
	}

	void ULowEntryJsonLibrary::Array_AddFloat(ULowEntryJsonArray* JsonArray, const float Value)
	{
		ULowEntryJsonArray::AddFloat(JsonArray, Value);
	}

	void ULowEntryJsonLibrary::Array_AddByte(ULowEntryJsonArray* JsonArray, const uint8 Value)
	{
		ULowEntryJsonArray::AddByte(JsonArray, Value);
	}

	void ULowEntryJsonLibrary::Array_AddBoolean(ULowEntryJsonArray* JsonArray, const bool Value)
	{
		ULowEntryJsonArray::AddBoolean(JsonArray, Value);
	}

	void ULowEntryJsonLibrary::Array_AddJsonObject(ULowEntryJsonArray* JsonArray, ULowEntryJsonObject* Value)
	{
		ULowEntryJsonArray::AddJsonObject(JsonArray, Value);
	}

	void ULowEntryJsonLibrary::Array_AddJsonArray(ULowEntryJsonArray* JsonArray, ULowEntryJsonArray* Value)
	{
		ULowEntryJsonArray::AddJsonArray(JsonArray, Value);
	}

	void ULowEntryJsonLibrary::Array_AddJsonValue(ULowEntryJsonArray* JsonArray, ULowEntryJsonValue* Value)
	{
		ULowEntryJsonArray::AddJsonValue(JsonArray, Value);
	}


	void ULowEntryJsonLibrary::Array_SetNull(ULowEntryJsonArray* JsonArray, const int32 Index)
	{
		ULowEntryJsonArray::SetNull(JsonArray, Index);
	}

	void ULowEntryJsonLibrary::Array_SetString(ULowEntryJsonArray* JsonArray, const int32 Index, const FString& Value)
	{
		ULowEntryJsonArray::SetString(JsonArray, Index, Value);
	}

	void ULowEntryJsonLibrary::Array_SetInteger(ULowEntryJsonArray* JsonArray, const int32 Index, const int32 Value)
	{
		ULowEntryJsonArray::SetInteger(JsonArray, Index, Value);
	}

	void ULowEntryJsonLibrary::Array_SetFloat(ULowEntryJsonArray* JsonArray, const int32 Index, const float Value)
	{
		ULowEntryJsonArray::SetFloat(JsonArray, Index, Value);
	}

	void ULowEntryJsonLibrary::Array_SetByte(ULowEntryJsonArray* JsonArray, const int32 Index, const uint8 Value)
	{
		ULowEntryJsonArray::SetByte(JsonArray, Index, Value);
	}

	void ULowEntryJsonLibrary::Array_SetBoolean(ULowEntryJsonArray* JsonArray, const int32 Index, const bool Value)
	{
		ULowEntryJsonArray::SetBoolean(JsonArray, Index, Value);
	}

	void ULowEntryJsonLibrary::Array_SetJsonObject(ULowEntryJsonArray* JsonArray, const int32 Index, ULowEntryJsonObject*  Value)
	{
		ULowEntryJsonArray::SetJsonObject(JsonArray, Index, Value);
	}

	void ULowEntryJsonLibrary::Array_SetJsonArray(ULowEntryJsonArray* JsonArray, const int32 Index, ULowEntryJsonArray* Value)
	{
		ULowEntryJsonArray::SetJsonArray(JsonArray, Index, Value);
	}

	void ULowEntryJsonLibrary::Array_SetJsonValue(ULowEntryJsonArray* JsonArray, const int32 Index, ULowEntryJsonValue* Value)
	{
		ULowEntryJsonArray::SetJsonValue(JsonArray, Index, Value);
	}


	void ULowEntryJsonLibrary::Array_InsertNull(ULowEntryJsonArray* JsonArray, const int32 Index)
	{
		ULowEntryJsonArray::InsertNull(JsonArray, Index);
	}

	void ULowEntryJsonLibrary::Array_InsertString(ULowEntryJsonArray* JsonArray, const int32 Index, const FString& Value)
	{
		ULowEntryJsonArray::InsertString(JsonArray, Index, Value);
	}

	void ULowEntryJsonLibrary::Array_InsertInteger(ULowEntryJsonArray* JsonArray, const int32 Index, const int32 Value)
	{
		ULowEntryJsonArray::InsertInteger(JsonArray, Index, Value);
	}

	void ULowEntryJsonLibrary::Array_InsertFloat(ULowEntryJsonArray* JsonArray, const int32 Index, const float Value)
	{
		ULowEntryJsonArray::InsertFloat(JsonArray, Index, Value);
	}

	void ULowEntryJsonLibrary::Array_InsertByte(ULowEntryJsonArray* JsonArray, const int32 Index, const uint8 Value)
	{
		ULowEntryJsonArray::InsertByte(JsonArray, Index, Value);
	}

	void ULowEntryJsonLibrary::Array_InsertBoolean(ULowEntryJsonArray* JsonArray, const int32 Index, const bool Value)
	{
		ULowEntryJsonArray::InsertBoolean(JsonArray, Index, Value);
	}

	void ULowEntryJsonLibrary::Array_InsertJsonObject(ULowEntryJsonArray* JsonArray, const int32 Index, ULowEntryJsonObject* Value)
	{
		ULowEntryJsonArray::InsertJsonObject(JsonArray, Index, Value);
	}

	void ULowEntryJsonLibrary::Array_InsertJsonArray(ULowEntryJsonArray* JsonArray, const int32 Index, ULowEntryJsonArray* Value)
	{
		ULowEntryJsonArray::InsertJsonArray(JsonArray, Index, Value);
	}

	void ULowEntryJsonLibrary::Array_InsertJsonValue(ULowEntryJsonArray* JsonArray, const int32 Index, ULowEntryJsonValue* Value)
	{
		ULowEntryJsonArray::InsertJsonValue(JsonArray, Index, Value);
	}


	void ULowEntryJsonLibrary::Array_Get(ULowEntryJsonArray* JsonArray, const int32 Index, ELowEntryJsonType& Branch, FString& ReturnStringValue, int32& ReturnIntegerValue, float& ReturnFloatValue, uint8& ReturnByteValue, bool& ReturnBooleanValue, ULowEntryJsonObject*& ReturnJsonObjectValue, ULowEntryJsonArray*& ReturnJsonArrayValue)
	{
		ULowEntryJsonArray::Get(JsonArray, Index, Branch, ReturnStringValue, ReturnIntegerValue, ReturnFloatValue, ReturnByteValue, ReturnBooleanValue, ReturnJsonObjectValue, ReturnJsonArrayValue);
	}

	void ULowEntryJsonLibrary::Array_GetNull(ULowEntryJsonArray* JsonArray, const int32 Index, ELowEntryJsonValueAndTypeFound& Branch)
	{
		ULowEntryJsonArray::GetNull(JsonArray, Index, Branch);
	}

	FString ULowEntryJsonLibrary::Array_GetString(ULowEntryJsonArray* JsonArray, const int32 Index, ELowEntryJsonValueAndTypeFound& Branch)
	{
		return ULowEntryJsonArray::GetString(JsonArray, Index, Branch);
	}

	int32 ULowEntryJsonLibrary::Array_GetInteger(ULowEntryJsonArray* JsonArray, const int32 Index, ELowEntryJsonValueAndTypeFound& Branch)
	{
		return ULowEntryJsonArray::GetInteger(JsonArray, Index, Branch);
	}

	float ULowEntryJsonLibrary::Array_GetFloat(ULowEntryJsonArray* JsonArray, const int32 Index, ELowEntryJsonValueAndTypeFound& Branch)
	{
		return ULowEntryJsonArray::GetFloat(JsonArray, Index, Branch);
	}

	uint8 ULowEntryJsonLibrary::Array_GetByte(ULowEntryJsonArray* JsonArray, const int32 Index, ELowEntryJsonValueAndTypeFound& Branch)
	{
		return ULowEntryJsonArray::GetByte(JsonArray, Index, Branch);
	}

	bool ULowEntryJsonLibrary::Array_GetBoolean(ULowEntryJsonArray* JsonArray, const int32 Index, ELowEntryJsonValueAndTypeFound& Branch)
	{
		return ULowEntryJsonArray::GetBoolean(JsonArray, Index, Branch);
	}

	ULowEntryJsonObject* ULowEntryJsonLibrary::Array_GetJsonObject(ULowEntryJsonArray* JsonArray, const int32 Index, ELowEntryJsonValueAndTypeFound& Branch)
	{
		return ULowEntryJsonArray::GetJsonObject(JsonArray, Index, Branch);
	}

	ULowEntryJsonArray* ULowEntryJsonLibrary::Array_GetJsonArray(ULowEntryJsonArray* JsonArray, const int32 Index, ELowEntryJsonValueAndTypeFound& Branch)
	{
		return ULowEntryJsonArray::GetJsonArray(JsonArray, Index, Branch);
	}

	ULowEntryJsonValue* ULowEntryJsonLibrary::Array_GetJsonValue(ULowEntryJsonArray* JsonArray, const int32 Index, ELowEntryJsonValueFound& Branch)
	{
		return ULowEntryJsonArray::GetJsonValue(JsonArray, Index, Branch);
	}


	FString ULowEntryJsonLibrary::Array_GetAsStringOrDefault(ULowEntryJsonArray* JsonArray, const int32 Index, const FString& Default)
	{
		return ULowEntryJsonArray::GetAsStringOrDefault(JsonArray, Index, Default);
	}

	int32 ULowEntryJsonLibrary::Array_GetAsIntegerOrDefault(ULowEntryJsonArray* JsonArray, const int32 Index, const int32 Default)
	{
		return ULowEntryJsonArray::GetAsIntegerOrDefault(JsonArray, Index, Default);
	}

	float ULowEntryJsonLibrary::Array_GetAsFloatOrDefault(ULowEntryJsonArray* JsonArray, const int32 Index, const float Default)
	{
		return ULowEntryJsonArray::GetAsFloatOrDefault(JsonArray, Index, Default);
	}

	uint8 ULowEntryJsonLibrary::Array_GetAsByteOrDefault(ULowEntryJsonArray* JsonArray, const int32 Index, const uint8 Default)
	{
		return ULowEntryJsonArray::GetAsByteOrDefault(JsonArray, Index, Default);
	}

	bool ULowEntryJsonLibrary::Array_GetAsBooleanOrDefault(ULowEntryJsonArray* JsonArray, const int32 Index, const bool Default)
	{
		return ULowEntryJsonArray::GetAsBooleanOrDefault(JsonArray, Index, Default);
	}

	ULowEntryJsonObject* ULowEntryJsonLibrary::Array_GetAsJsonObjectOrDefault(ULowEntryJsonArray* JsonArray, const int32 Index, ULowEntryJsonObject* Default)
	{
		return ULowEntryJsonArray::GetAsJsonObjectOrDefault(JsonArray, Index, Default);
	}

	ULowEntryJsonArray* ULowEntryJsonLibrary::Array_GetAsJsonArrayOrDefault(ULowEntryJsonArray* JsonArray, const int32 Index, ULowEntryJsonArray* Default)
	{
		return ULowEntryJsonArray::GetAsJsonArrayOrDefault(JsonArray, Index, Default);
	}

	ULowEntryJsonValue* ULowEntryJsonLibrary::Array_GetAsJsonValueOrDefault(ULowEntryJsonArray* JsonArray, const int32 Index, ULowEntryJsonValue* Default)
	{
		return ULowEntryJsonArray::GetAsJsonValueOrDefault(JsonArray, Index, Default);
	}


	int32 ULowEntryJsonLibrary::Array_Length(ULowEntryJsonArray* JsonArray)
	{
		return ULowEntryJsonArray::Length(JsonArray);
	}


	void ULowEntryJsonLibrary::Array_Resize(ULowEntryJsonArray* JsonArray, const int32 Size)
	{
		ULowEntryJsonArray::Resize(JsonArray, Size);
	}

	void ULowEntryJsonLibrary::Array_Expand(ULowEntryJsonArray* JsonArray, const int32 Size)
	{
		ULowEntryJsonArray::Expand(JsonArray, Size);
	}

	void ULowEntryJsonLibrary::Array_Shorten(ULowEntryJsonArray* JsonArray, const int32 Size)
	{
		ULowEntryJsonArray::Shorten(JsonArray, Size);
	}


	void ULowEntryJsonLibrary::Array_Append(ULowEntryJsonArray* JsonArray, ULowEntryJsonArray* AppendJsonArray)
	{
		ULowEntryJsonArray::Append(JsonArray, AppendJsonArray);
	}
// array <<



// value >>
	ULowEntryJsonValue* ULowEntryJsonLibrary::Value_Create()
	{
		return ULowEntryJsonValue::Create();
	}

	ULowEntryJsonValue* ULowEntryJsonLibrary::Value_CreateFromNull()
	{
		return ULowEntryJsonValue::CreateFromNull();
	}

	ULowEntryJsonValue* ULowEntryJsonLibrary::Value_CreateFromString(const FString& Value)
	{
		return ULowEntryJsonValue::CreateFromString(Value);
	}

	ULowEntryJsonValue* ULowEntryJsonLibrary::Value_CreateFromInteger(const int32 Value)
	{
		return ULowEntryJsonValue::CreateFromInteger(Value);
	}

	ULowEntryJsonValue* ULowEntryJsonLibrary::Value_CreateFromFloat(const float Value)
	{
		return ULowEntryJsonValue::CreateFromFloat(Value);
	}

	ULowEntryJsonValue* ULowEntryJsonLibrary::Value_CreateFromByte(const uint8 Value)
	{
		return ULowEntryJsonValue::CreateFromByte(Value);
	}

	ULowEntryJsonValue* ULowEntryJsonLibrary::Value_CreateFromBoolean(const bool Value)
	{
		return ULowEntryJsonValue::CreateFromBoolean(Value);
	}

	ULowEntryJsonValue* ULowEntryJsonLibrary::Value_CreateFromJsonObject(ULowEntryJsonObject* Value)
	{
		return ULowEntryJsonValue::CreateFromJsonObject(Value);
	}

	ULowEntryJsonValue* ULowEntryJsonLibrary::Value_CreateFromJsonArray(ULowEntryJsonArray* Value)
	{
		return ULowEntryJsonValue::CreateFromJsonArray(Value);
	}

	ULowEntryJsonValue* ULowEntryJsonLibrary::Value_Clone(ULowEntryJsonValue* JsonValue)
	{
		return ULowEntryJsonValue::Clone(JsonValue);
	}


	ULowEntryJsonValue* ULowEntryJsonLibrary::Value_Pure_Create()
	{
		return ULowEntryJsonValue::Create();
	}

	ULowEntryJsonValue* ULowEntryJsonLibrary::Value_Pure_CreateFromNull()
	{
		return ULowEntryJsonValue::CreateFromNull();
	}

	ULowEntryJsonValue* ULowEntryJsonLibrary::Value_Pure_CreateFromString(const FString& Value)
	{
		return ULowEntryJsonValue::CreateFromString(Value);
	}

	ULowEntryJsonValue* ULowEntryJsonLibrary::Value_Pure_CreateFromInteger(const int32 Value)
	{
		return ULowEntryJsonValue::CreateFromInteger(Value);
	}

	ULowEntryJsonValue* ULowEntryJsonLibrary::Value_Pure_CreateFromFloat(const float Value)
	{
		return ULowEntryJsonValue::CreateFromFloat(Value);
	}

	ULowEntryJsonValue* ULowEntryJsonLibrary::Value_Pure_CreateFromByte(const uint8 Value)
	{
		return ULowEntryJsonValue::CreateFromByte(Value);
	}

	ULowEntryJsonValue* ULowEntryJsonLibrary::Value_Pure_CreateFromBoolean(const bool Value)
	{
		return ULowEntryJsonValue::CreateFromBoolean(Value);
	}

	ULowEntryJsonValue* ULowEntryJsonLibrary::Value_Pure_CreateFromJsonObject(ULowEntryJsonObject* Value)
	{
		return ULowEntryJsonValue::CreateFromJsonObject(Value);
	}

	ULowEntryJsonValue* ULowEntryJsonLibrary::Value_Pure_CreateFromJsonArray(ULowEntryJsonArray* Value)
	{
		return ULowEntryJsonValue::CreateFromJsonArray(Value);
	}

	ULowEntryJsonValue* ULowEntryJsonLibrary::Value_Pure_Clone(ULowEntryJsonValue* JsonValue)
	{
		return ULowEntryJsonValue::Clone(JsonValue);
	}


	void ULowEntryJsonLibrary::Value_Set(ULowEntryJsonValue* JsonValue, ULowEntryJsonValue* Value)
	{
		ULowEntryJsonValue::Set(JsonValue, Value);
	}

	void ULowEntryJsonLibrary::Value_SetNull(ULowEntryJsonValue* JsonValue)
	{
		ULowEntryJsonValue::SetNull(JsonValue);
	}

	void ULowEntryJsonLibrary::Value_SetString(ULowEntryJsonValue* JsonValue, const FString& Value)
	{
		ULowEntryJsonValue::SetString(JsonValue, Value);
	}

	void ULowEntryJsonLibrary::Value_SetInteger(ULowEntryJsonValue* JsonValue, const int32 Value)
	{
		ULowEntryJsonValue::SetInteger(JsonValue, Value);
	}

	void ULowEntryJsonLibrary::Value_SetFloat(ULowEntryJsonValue* JsonValue, const float Value)
	{
		ULowEntryJsonValue::SetFloat(JsonValue, Value);
	}

	void ULowEntryJsonLibrary::Value_SetByte(ULowEntryJsonValue* JsonValue, const uint8 Value)
	{
		ULowEntryJsonValue::SetByte(JsonValue, Value);
	}

	void ULowEntryJsonLibrary::Value_SetBoolean(ULowEntryJsonValue* JsonValue, const bool Value)
	{
		ULowEntryJsonValue::SetBoolean(JsonValue, Value);
	}

	void ULowEntryJsonLibrary::Value_SetJsonObject(ULowEntryJsonValue* JsonValue, ULowEntryJsonObject* Value)
	{
		ULowEntryJsonValue::SetJsonObject(JsonValue, Value);
	}

	void ULowEntryJsonLibrary::Value_SetJsonArray(ULowEntryJsonValue* JsonValue, ULowEntryJsonArray* Value)
	{
		ULowEntryJsonValue::SetJsonArray(JsonValue, Value);
	}


	void ULowEntryJsonLibrary::Value_Get(ULowEntryJsonValue* JsonValue, ELowEntryJsonType& Branch, FString& ReturnStringValue, int32& ReturnIntegerValue, float& ReturnFloatValue, uint8& ReturnByteValue, bool& ReturnBooleanValue, ULowEntryJsonObject*& ReturnJsonObjectValue, ULowEntryJsonArray*& ReturnJsonArrayValue)
	{
		ULowEntryJsonValue::Get(JsonValue, Branch, ReturnStringValue, ReturnIntegerValue, ReturnFloatValue, ReturnByteValue, ReturnBooleanValue, ReturnJsonObjectValue, ReturnJsonArrayValue);
	}

	void ULowEntryJsonLibrary::Value_GetNull(ULowEntryJsonValue* JsonValue, ELowEntryJsonTypeFound& Branch)
	{
		ULowEntryJsonValue::GetNull(JsonValue, Branch);
	}

	FString ULowEntryJsonLibrary::Value_GetString(ULowEntryJsonValue* JsonValue, ELowEntryJsonTypeFound& Branch)
	{
		return ULowEntryJsonValue::GetString(JsonValue, Branch);
	}

	int32 ULowEntryJsonLibrary::Value_GetInteger(ULowEntryJsonValue* JsonValue, ELowEntryJsonTypeFound& Branch)
	{
		return ULowEntryJsonValue::GetInteger(JsonValue, Branch);
	}

	float ULowEntryJsonLibrary::Value_GetFloat(ULowEntryJsonValue* JsonValue, ELowEntryJsonTypeFound& Branch)
	{
		return ULowEntryJsonValue::GetFloat(JsonValue, Branch);
	}

	uint8 ULowEntryJsonLibrary::Value_GetByte(ULowEntryJsonValue* JsonValue, ELowEntryJsonTypeFound& Branch)
	{
		return ULowEntryJsonValue::GetByte(JsonValue, Branch);
	}

	bool ULowEntryJsonLibrary::Value_GetBoolean(ULowEntryJsonValue* JsonValue, ELowEntryJsonTypeFound& Branch)
	{
		return ULowEntryJsonValue::GetBoolean(JsonValue, Branch);
	}

	ULowEntryJsonObject* ULowEntryJsonLibrary::Value_GetJsonObject(ULowEntryJsonValue* JsonValue, ELowEntryJsonTypeFound& Branch)
	{
		return ULowEntryJsonValue::GetJsonObject(JsonValue, Branch);
	}

	ULowEntryJsonArray* ULowEntryJsonLibrary::Value_GetJsonArray(ULowEntryJsonValue* JsonValue, ELowEntryJsonTypeFound& Branch)
	{
		return ULowEntryJsonValue::GetJsonArray(JsonValue, Branch);
	}


	FString ULowEntryJsonLibrary::Value_GetAsStringOrDefault(ULowEntryJsonValue* JsonValue, const FString& Default)
	{
		return ULowEntryJsonValue::GetAsStringOrDefault(JsonValue, Default);
	}

	int32 ULowEntryJsonLibrary::Value_GetAsIntegerOrDefault(ULowEntryJsonValue* JsonValue, const int32 Default)
	{
		return ULowEntryJsonValue::GetAsIntegerOrDefault(JsonValue, Default);
	}

	float ULowEntryJsonLibrary::Value_GetAsFloatOrDefault(ULowEntryJsonValue* JsonValue, const float Default)
	{
		return ULowEntryJsonValue::GetAsFloatOrDefault(JsonValue, Default);
	}

	uint8 ULowEntryJsonLibrary::Value_GetAsByteOrDefault(ULowEntryJsonValue* JsonValue, const uint8 Default)
	{
		return ULowEntryJsonValue::GetAsByteOrDefault(JsonValue, Default);
	}

	bool ULowEntryJsonLibrary::Value_GetAsBooleanOrDefault(ULowEntryJsonValue* JsonValue, const bool Default)
	{
		return ULowEntryJsonValue::GetAsBooleanOrDefault(JsonValue, Default);
	}

	ULowEntryJsonObject* ULowEntryJsonLibrary::Value_GetAsJsonObjectOrDefault(ULowEntryJsonValue* JsonValue, ULowEntryJsonObject* Default)
	{
		return ULowEntryJsonValue::GetAsJsonObjectOrDefault(JsonValue, Default);
	}

	ULowEntryJsonArray* ULowEntryJsonLibrary::Value_GetAsJsonArrayOrDefault(ULowEntryJsonValue* JsonValue, ULowEntryJsonArray* Default)
	{
		return ULowEntryJsonValue::GetAsJsonArrayOrDefault(JsonValue, Default);
	}
// value <<
