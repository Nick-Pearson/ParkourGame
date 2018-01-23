#include "LowEntryJsonValue.h"
#include "LowEntryJsonObject.h"
#include "LowEntryJsonArray.h"


// init >>
	ULowEntryJsonValue::ULowEntryJsonValue(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
	{
		Data = MakeShareable(new FJsonValueNull());
	}

	ULowEntryJsonValue* ULowEntryJsonValue::Create()
	{
		return NewObject<ULowEntryJsonValue>();
	}

	ULowEntryJsonValue* ULowEntryJsonValue::Create(const TSharedPtr<FJsonValue> Value)
	{
		ULowEntryJsonValue* Instance = NewObject<ULowEntryJsonValue>();
		Instance->Data = ULowEntryJsonValue::CloneData(Value);
		return Instance;
	}

	ULowEntryJsonValue* ULowEntryJsonValue::CreateFromNull()
	{
		ULowEntryJsonValue* Instance = NewObject<ULowEntryJsonValue>();
		Instance->Data = MakeShareable(new FJsonValueNull());
		return Instance;
	}

	ULowEntryJsonValue* ULowEntryJsonValue::CreateFromString(const FString& Value)
	{
		ULowEntryJsonValue* Instance = NewObject<ULowEntryJsonValue>();
		Instance->Data = MakeShareable(new FJsonValueString(Value));
		return Instance;
	}

	ULowEntryJsonValue* ULowEntryJsonValue::CreateFromInteger(const int32 Value)
	{
		ULowEntryJsonValue* Instance = NewObject<ULowEntryJsonValue>();
		Instance->Data = MakeShareable(new FJsonValueNumber(Value));
		return Instance;
	}

	ULowEntryJsonValue* ULowEntryJsonValue::CreateFromFloat(const float Value)
	{
		ULowEntryJsonValue* Instance = NewObject<ULowEntryJsonValue>();
		Instance->Data = MakeShareable(new FJsonValueNumber(Value));
		return Instance;
	}

	ULowEntryJsonValue* ULowEntryJsonValue::CreateFromByte(const uint8 Value)
	{
		ULowEntryJsonValue* Instance = NewObject<ULowEntryJsonValue>();
		Instance->Data = MakeShareable(new FJsonValueNumber(Value));
		return Instance;
	}

	ULowEntryJsonValue* ULowEntryJsonValue::CreateFromBoolean(const bool Value)
	{
		ULowEntryJsonValue* Instance = NewObject<ULowEntryJsonValue>();
		Instance->Data = MakeShareable(new FJsonValueBoolean(Value));
		return Instance;
	}

	ULowEntryJsonValue* ULowEntryJsonValue::CreateFromJsonObject(ULowEntryJsonObject* Value)
	{
		ULowEntryJsonValue* Instance = NewObject<ULowEntryJsonValue>();
		if(Value != nullptr)
		{
			Instance->Data = MakeShareable(new FJsonValueObject(ULowEntryJsonObject::CloneData(Value->Data)));
		}
		else
		{
			Instance->Data = MakeShareable(new FJsonValueNull());
		}
		return Instance;
	}

	ULowEntryJsonValue* ULowEntryJsonValue::CreateFromJsonArray(ULowEntryJsonArray* Value)
	{
		ULowEntryJsonValue* Instance = NewObject<ULowEntryJsonValue>();
		if(Value != nullptr)
		{
			Instance->Data = MakeShareable(new FJsonValueArray(Value->Data));
		}
		else
		{
			Instance->Data = MakeShareable(new FJsonValueNull());
		}
		return Instance;
	}

	ULowEntryJsonValue* ULowEntryJsonValue::Clone(ULowEntryJsonValue* JsonValue)
	{
		if(JsonValue != nullptr)
		{
			return ULowEntryJsonValue::Create(JsonValue->Data);
		}
		else
		{
			return ULowEntryJsonValue::Create();
		}
	}

	TSharedPtr<FJsonValue> ULowEntryJsonValue::CloneData(const TSharedPtr<FJsonValue> Data)
	{
		if(!Data.IsValid() || (Data->Type == EJson::Null))
		{
			return MakeShareable(new FJsonValueNull());
		}
		else if(Data->Type == EJson::String)
		{
			return MakeShareable(new FJsonValueString(Data->AsString()));
		}
		else if(Data->Type == EJson::Number)
		{
			return MakeShareable(new FJsonValueNumber(Data->AsNumber()));
		}
		else if(Data->Type == EJson::Boolean)
		{
			return MakeShareable(new FJsonValueBoolean(Data->AsBool()));
		}
		else if(Data->Type == EJson::Object)
		{
			return MakeShareable(new FJsonValueObject(ULowEntryJsonObject::CloneData(Data->AsObject())));
		}
		else if(Data->Type == EJson::Array)
		{
			return MakeShareable(new FJsonValueArray(Data->AsArray()));
		}
		return MakeShareable(new FJsonValueNull());
	}
// init <<


// json >>
	void ULowEntryJsonValue::AddToJson(const TSharedPtr<FJsonValue> Data, const TSharedRef<TJsonWriter<TCHAR, TPrettyJsonPrintPolicy<TCHAR>>> JsonWriter)
	{
		if(!Data.IsValid() || (Data->Type == EJson::Null))
		{
			JsonWriter->WriteNull();
		}
		else if(Data->Type == EJson::String)
		{
			JsonWriter->WriteValue(Data->AsString());
		}
		else if(Data->Type == EJson::Number)
		{
			JsonWriter->WriteValue(Data->AsNumber());
		}
		else if(Data->Type == EJson::Boolean)
		{
			JsonWriter->WriteValue(Data->AsBool());
		}
		else if(Data->Type == EJson::Object)
		{
			ULowEntryJsonObject::AddToJson(Data->AsObject(), JsonWriter);
		}
		else if(Data->Type == EJson::Array)
		{
			ULowEntryJsonArray::AddToJson(Data->AsArray(), JsonWriter);
		}
	}
	void ULowEntryJsonValue::AddToJson(const FString& Key, const TSharedPtr<FJsonValue> Data, const TSharedRef<TJsonWriter<TCHAR, TPrettyJsonPrintPolicy<TCHAR>>> JsonWriter)
	{
		if(!Data.IsValid() || (Data->Type == EJson::Null))
		{
			JsonWriter->WriteNull(Key);
		}
		else if(Data->Type == EJson::String)
		{
			JsonWriter->WriteValue(Key, Data->AsString());
		}
		else if(Data->Type == EJson::Number)
		{
			JsonWriter->WriteValue(Key, Data->AsNumber());
		}
		else if(Data->Type == EJson::Boolean)
		{
			JsonWriter->WriteValue(Key, Data->AsBool());
		}
		else if(Data->Type == EJson::Object)
		{
			ULowEntryJsonObject::AddToJson(Key, Data->AsObject(), JsonWriter);
		}
		else if(Data->Type == EJson::Array)
		{
			ULowEntryJsonArray::AddToJson(Key, Data->AsArray(), JsonWriter);
		}
	}

	void ULowEntryJsonValue::AddToJson(const TSharedPtr<FJsonValue> Data, const TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
	{
		if(!Data.IsValid() || (Data->Type == EJson::Null))
		{
			JsonWriter->WriteNull();
		}
		else if(Data->Type == EJson::String)
		{
			JsonWriter->WriteValue(Data->AsString());
		}
		else if(Data->Type == EJson::Number)
		{
			JsonWriter->WriteValue(Data->AsNumber());
		}
		else if(Data->Type == EJson::Boolean)
		{
			JsonWriter->WriteValue(Data->AsBool());
		}
		else if(Data->Type == EJson::Object)
		{
			ULowEntryJsonObject::AddToJson(Data->AsObject(), JsonWriter);
		}
		else if(Data->Type == EJson::Array)
		{
			ULowEntryJsonArray::AddToJson(Data->AsArray(), JsonWriter);
		}
	}
	void ULowEntryJsonValue::AddToJson(const FString& Key, const TSharedPtr<FJsonValue> Data, const TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
	{
		if(!Data.IsValid() || (Data->Type == EJson::Null))
		{
			JsonWriter->WriteNull(Key);
		}
		else if(Data->Type == EJson::String)
		{
			JsonWriter->WriteValue(Key, Data->AsString());
		}
		else if(Data->Type == EJson::Number)
		{
			JsonWriter->WriteValue(Key, Data->AsNumber());
		}
		else if(Data->Type == EJson::Boolean)
		{
			JsonWriter->WriteValue(Key, Data->AsBool());
		}
		else if(Data->Type == EJson::Object)
		{
			ULowEntryJsonObject::AddToJson(Key, Data->AsObject(), JsonWriter);
		}
		else if(Data->Type == EJson::Array)
		{
			ULowEntryJsonArray::AddToJson(Key, Data->AsArray(), JsonWriter);
		}
	}
// json <<


void ULowEntryJsonValue::Set(ULowEntryJsonValue* JsonValue, ULowEntryJsonValue* Value)
{
	if(JsonValue == nullptr)
	{
		return;
	}
	if(Value != nullptr)
	{
		JsonValue->Data = ULowEntryJsonValue::CloneData(Value->Data);
	}
	else
	{
		JsonValue->Data = MakeShareable(new FJsonValueNull());
	}
}

void ULowEntryJsonValue::SetNull(ULowEntryJsonValue* JsonValue)
{
	if(JsonValue == nullptr)
	{
		return;
	}
	JsonValue->Data = MakeShareable(new FJsonValueNull());
}

void ULowEntryJsonValue::SetString(ULowEntryJsonValue* JsonValue, const FString& Value)
{
	if(JsonValue == nullptr)
	{
		return;
	}
	JsonValue->Data = MakeShareable(new FJsonValueString(Value));
}

void ULowEntryJsonValue::SetInteger(ULowEntryJsonValue* JsonValue, const int32 Value)
{
	if(JsonValue == nullptr)
	{
		return;
	}
	JsonValue->Data = MakeShareable(new FJsonValueNumber(Value));
}

void ULowEntryJsonValue::SetFloat(ULowEntryJsonValue* JsonValue, const float Value)
{
	if(JsonValue == nullptr)
	{
		return;
	}
	JsonValue->Data = MakeShareable(new FJsonValueNumber(Value));
}

void ULowEntryJsonValue::SetByte(ULowEntryJsonValue* JsonValue, const uint8 Value)
{
	if(JsonValue == nullptr)
	{
		return;
	}
	JsonValue->Data = MakeShareable(new FJsonValueNumber(Value));
}

void ULowEntryJsonValue::SetBoolean(ULowEntryJsonValue* JsonValue, const bool Value)
{
	if(JsonValue == nullptr)
	{
		return;
	}
	JsonValue->Data = MakeShareable(new FJsonValueBoolean(Value));
}

void ULowEntryJsonValue::SetJsonObject(ULowEntryJsonValue* JsonValue, ULowEntryJsonObject* Value)
{
	if(JsonValue == nullptr)
	{
		return;
	}
	if(Value != nullptr)
	{
		JsonValue->Data = MakeShareable(new FJsonValueObject(ULowEntryJsonObject::CloneData(Value->Data)));
	}
	else
	{
		JsonValue->Data = MakeShareable(new FJsonValueNull());
	}
}

void ULowEntryJsonValue::SetJsonArray(ULowEntryJsonValue* JsonValue, ULowEntryJsonArray* Value)
{
	if(JsonValue == nullptr)
	{
		return;
	}
	if(Value != nullptr)
	{
		JsonValue->Data = MakeShareable(new FJsonValueArray(Value->Data));
	}
	else
	{
		JsonValue->Data = MakeShareable(new FJsonValueNull());
	}
}


void ULowEntryJsonValue::Get(ULowEntryJsonValue* JsonValue, ELowEntryJsonType& Branch, FString& ReturnStringValue, int32& ReturnIntegerValue, float& ReturnFloatValue, uint8& ReturnByteValue, bool& ReturnBooleanValue, ULowEntryJsonObject*& ReturnJsonObjectValue, ULowEntryJsonArray*& ReturnJsonArrayValue)
{
	if(JsonValue == nullptr)
	{
		Branch = ELowEntryJsonType::None;
		return;
	}
	if(JsonValue->Data->Type == EJson::None)
	{
		Branch = ELowEntryJsonType::None;
	}
	else if(JsonValue->Data->Type == EJson::Null)
	{
		Branch = ELowEntryJsonType::Null;
	}
	else if(JsonValue->Data->Type == EJson::String)
	{
		ReturnStringValue = JsonValue->Data->AsString();
		Branch = ELowEntryJsonType::String;
	}
	else if(JsonValue->Data->Type == EJson::Number)
	{
		ReturnFloatValue = JsonValue->Data->AsNumber();
		float Remainder = FMath::Fmod(ReturnFloatValue, 1);
		if((Remainder > 0.0001) || (Remainder < -0.0001))
		{
			Branch = ELowEntryJsonType::Float;
			return;
		}
		ReturnIntegerValue = FMath::RoundToInt(ReturnFloatValue);
		if((ReturnIntegerValue < 0) || (ReturnIntegerValue > 255))
		{
			Branch = ELowEntryJsonType::IntegerOrFloat;
			return;
		}
		ReturnByteValue = (uint8) ReturnIntegerValue;
		Branch = ELowEntryJsonType::ByteOrIntegerOrFloat;
	}
	else if(JsonValue->Data->Type == EJson::Boolean)
	{
		ReturnBooleanValue = JsonValue->Data->AsBool();
		Branch = ELowEntryJsonType::Boolean;
	}
	else if(JsonValue->Data->Type == EJson::Object)
	{
		ReturnJsonObjectValue = ULowEntryJsonObject::Create(JsonValue->Data->AsObject());
		Branch = ELowEntryJsonType::JsonObject;
	}
	else if(JsonValue->Data->Type == EJson::Array)
	{
		ReturnJsonArrayValue = ULowEntryJsonArray::Create(JsonValue->Data->AsArray());
		Branch = ELowEntryJsonType::JsonArray;
	}
	else
	{
		Branch = ELowEntryJsonType::None;
	}
}

void ULowEntryJsonValue::GetNull(ULowEntryJsonValue* JsonValue, ELowEntryJsonTypeFound& Branch)
{
	if(JsonValue == nullptr)
	{
		Branch = ELowEntryJsonTypeFound::WrongType;
		return;
	}
	if(JsonValue->Data->Type == EJson::Null)
	{
		Branch = ELowEntryJsonTypeFound::Success;
		return;
	}
	Branch = ELowEntryJsonTypeFound::WrongType;
	return;
}

FString ULowEntryJsonValue::GetString(ULowEntryJsonValue* JsonValue, ELowEntryJsonTypeFound& Branch)
{
	if(JsonValue == nullptr)
	{
		Branch = ELowEntryJsonTypeFound::WrongType;
		return TEXT("");
	}
	if(JsonValue->Data->Type == EJson::String)
	{
		Branch = ELowEntryJsonTypeFound::Success;
		return JsonValue->Data->AsString();
	}
	Branch = ELowEntryJsonTypeFound::WrongType;
	return TEXT("");
}

int32 ULowEntryJsonValue::GetInteger(ULowEntryJsonValue* JsonValue, ELowEntryJsonTypeFound& Branch)
{
	if(JsonValue == nullptr)
	{
		Branch = ELowEntryJsonTypeFound::WrongType;
		return 0;
	}
	if(JsonValue->Data->Type == EJson::Number)
	{
		float ReturnFloatValue = JsonValue->Data->AsNumber();
		float Remainder = FMath::Fmod(ReturnFloatValue, 1);
		if((Remainder <= 0.0001) && (Remainder >= -0.0001))
		{
			Branch = ELowEntryJsonTypeFound::Success;
			return FMath::RoundToInt(ReturnFloatValue);
		}
	}
	Branch = ELowEntryJsonTypeFound::WrongType;
	return 0;
}

float ULowEntryJsonValue::GetFloat(ULowEntryJsonValue* JsonValue, ELowEntryJsonTypeFound& Branch)
{
	if(JsonValue == nullptr)
	{
		Branch = ELowEntryJsonTypeFound::WrongType;
		return 0;
	}
	if(JsonValue->Data->Type == EJson::Number)
	{
		Branch = ELowEntryJsonTypeFound::Success;
		return JsonValue->Data->AsNumber();
	}
	Branch = ELowEntryJsonTypeFound::WrongType;
	return 0;
}

uint8 ULowEntryJsonValue::GetByte(ULowEntryJsonValue* JsonValue, ELowEntryJsonTypeFound& Branch)
{
	if(JsonValue == nullptr)
	{
		Branch = ELowEntryJsonTypeFound::WrongType;
		return 0;
	}
	if(JsonValue->Data->Type == EJson::Number)
	{
		float ReturnFloatValue = JsonValue->Data->AsNumber();
		float Remainder = FMath::Fmod(ReturnFloatValue, 1);
		if((Remainder <= 0.0001) && (Remainder >= -0.0001))
		{
			int32 ReturnIntegerValue = FMath::RoundToInt(ReturnFloatValue);
			if((ReturnIntegerValue >= 0) && (ReturnIntegerValue <= 255))
			{
				Branch = ELowEntryJsonTypeFound::Success;
				return (uint8) ReturnIntegerValue;
			}
		}
	}
	Branch = ELowEntryJsonTypeFound::WrongType;
	return 0;
}

bool ULowEntryJsonValue::GetBoolean(ULowEntryJsonValue* JsonValue, ELowEntryJsonTypeFound& Branch)
{
	if(JsonValue == nullptr)
	{
		Branch = ELowEntryJsonTypeFound::WrongType;
		return false;
	}
	if(JsonValue->Data->Type == EJson::Boolean)
	{
		Branch = ELowEntryJsonTypeFound::Success;
		return JsonValue->Data->AsBool();
	}
	Branch = ELowEntryJsonTypeFound::WrongType;
	return false;
}

ULowEntryJsonObject* ULowEntryJsonValue::GetJsonObject(ULowEntryJsonValue* JsonValue, ELowEntryJsonTypeFound& Branch)
{
	if(JsonValue == nullptr)
	{
		Branch = ELowEntryJsonTypeFound::WrongType;
		return nullptr;
	}
	if(JsonValue->Data->Type == EJson::Object)
	{
		Branch = ELowEntryJsonTypeFound::Success;
		return ULowEntryJsonObject::Create(JsonValue->Data->AsObject());
	}
	Branch = ELowEntryJsonTypeFound::WrongType;
	return nullptr;
}

ULowEntryJsonArray* ULowEntryJsonValue::GetJsonArray(ULowEntryJsonValue* JsonValue, ELowEntryJsonTypeFound& Branch)
{
	if(JsonValue == nullptr)
	{
		Branch = ELowEntryJsonTypeFound::WrongType;
		return nullptr;
	}
	if(JsonValue->Data->Type == EJson::Array)
	{
		Branch = ELowEntryJsonTypeFound::Success;
		return ULowEntryJsonArray::Create(JsonValue->Data->AsArray());
	}
	Branch = ELowEntryJsonTypeFound::WrongType;
	return nullptr;
}


FString ULowEntryJsonValue::GetAsStringOrDefault(ULowEntryJsonValue* JsonValue, const FString& Default)
{
	if(JsonValue == nullptr)
	{
		return Default;
	}
	FString Value;
	if(JsonValue->Data->TryGetString(Value))
	{
		return Value;
	}
	return Default;
}

int32 ULowEntryJsonValue::GetAsIntegerOrDefault(ULowEntryJsonValue* JsonValue, const int32 Default)
{
	if(JsonValue == nullptr)
	{
		return Default;
	}
	int32 Value;
	if(JsonValue->Data->TryGetNumber(Value))
	{
		return Value;
	}
	return Default;
}

float ULowEntryJsonValue::GetAsFloatOrDefault(ULowEntryJsonValue* JsonValue, const float Default)
{
	if(JsonValue == nullptr)
	{
		return Default;
	}
	double Value;
	if(JsonValue->Data->TryGetNumber(Value))
	{
		return (float) Value;
	}
	return Default;
}

uint8 ULowEntryJsonValue::GetAsByteOrDefault(ULowEntryJsonValue* JsonValue, const uint8 Default)
{
	if(JsonValue == nullptr)
	{
		return Default;
	}
	int32 Value;
	if(JsonValue->Data->TryGetNumber(Value))
	{
		if((Value >= 0) && (Value <= 255))
		{
			return (uint8) Value;
		}
	}
	return Default;
}

bool ULowEntryJsonValue::GetAsBooleanOrDefault(ULowEntryJsonValue* JsonValue, const bool Default)
{
	if(JsonValue == nullptr)
	{
		return Default;
	}
	bool Value;
	if(JsonValue->Data->TryGetBool(Value))
	{
		return Value;
	}
	return Default;
}

ULowEntryJsonObject* ULowEntryJsonValue::GetAsJsonObjectOrDefault(ULowEntryJsonValue* JsonValue, ULowEntryJsonObject* Default)
{
	//if(Default == nullptr)
	//{
	//	Default = ULowEntryJsonObject::Create();
	//}
	if(JsonValue == nullptr)
	{
		return Default;
	}
	const TSharedPtr<FJsonObject>* Value;
	if(JsonValue->Data->TryGetObject(Value))
	{
		return ULowEntryJsonObject::Create(*Value);
	}
	return Default;
}

ULowEntryJsonArray* ULowEntryJsonValue::GetAsJsonArrayOrDefault(ULowEntryJsonValue* JsonValue, ULowEntryJsonArray* Default)
{
	//if(Default == nullptr)
	//{
	//	Default = ULowEntryJsonArray::Create();
	//}
	if(JsonValue == nullptr)
	{
		return Default;
	}
	const TArray<TSharedPtr<FJsonValue>>* Value;
	if(JsonValue->Data->TryGetArray(Value))
	{
		return ULowEntryJsonArray::Create(*Value);
	}
	return Default;
}
