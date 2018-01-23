#include "LowEntryJsonArray.h"
#include "LowEntryJsonObject.h"
#include "LowEntryJsonValue.h"


// init >>
	ULowEntryJsonArray::ULowEntryJsonArray(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
	{
	}

	ULowEntryJsonArray* ULowEntryJsonArray::Create()
	{
		return NewObject<ULowEntryJsonArray>();
	}

	ULowEntryJsonArray* ULowEntryJsonArray::Create(const TArray<TSharedPtr<FJsonValue>> Value)
	{
		ULowEntryJsonArray* Instance = NewObject<ULowEntryJsonArray>();
		Instance->Data = Value;
		return Instance;
	}

	ULowEntryJsonArray* ULowEntryJsonArray::CreateFromStringArray(const TArray<FString>& Value)
	{
		ULowEntryJsonArray* Instance = NewObject<ULowEntryJsonArray>();
		for(int32 i = 0; i < Value.Num(); i++)
		{
			Instance->Data.Add(MakeShareable(new FJsonValueString(Value[i])));
		}
		return Instance;
	}

	ULowEntryJsonArray* ULowEntryJsonArray::CreateFromIntegerArray(const TArray<int32>& Value)
	{
		ULowEntryJsonArray* Instance = NewObject<ULowEntryJsonArray>();
		for(int32 i = 0; i < Value.Num(); i++)
		{
			Instance->Data.Add(MakeShareable(new FJsonValueNumber(Value[i])));
		}
		return Instance;
	}

	ULowEntryJsonArray* ULowEntryJsonArray::CreateFromFloatArray(const TArray<float>& Value)
	{
		ULowEntryJsonArray* Instance = NewObject<ULowEntryJsonArray>();
		for(int32 i = 0; i < Value.Num(); i++)
		{
			Instance->Data.Add(MakeShareable(new FJsonValueNumber(Value[i])));
		}
		return Instance;
	}

	ULowEntryJsonArray* ULowEntryJsonArray::CreateFromByteArray(const TArray<uint8>& Value)
	{
		ULowEntryJsonArray* Instance = NewObject<ULowEntryJsonArray>();
		for(int32 i = 0; i < Value.Num(); i++)
		{
			Instance->Data.Add(MakeShareable(new FJsonValueNumber(Value[i])));
		}
		return Instance;
	}

	ULowEntryJsonArray* ULowEntryJsonArray::CreateFromBooleanArray(const TArray<bool>& Value)
	{
		ULowEntryJsonArray* Instance = NewObject<ULowEntryJsonArray>();
		for(int32 i = 0; i < Value.Num(); i++)
		{
			Instance->Data.Add(MakeShareable(new FJsonValueBoolean(Value[i])));
		}
		return Instance;
	}

	ULowEntryJsonArray* ULowEntryJsonArray::CreateFromJsonObjectArray(const TArray<ULowEntryJsonObject*>& Value)
	{
		ULowEntryJsonArray* Instance = NewObject<ULowEntryJsonArray>();
		for(int32 i = 0; i < Value.Num(); i++)
		{
			if(Value[i] != nullptr)
			{
				Instance->Data.Add(MakeShareable(new FJsonValueObject(ULowEntryJsonObject::CloneData(Value[i]->Data))));
			}
		}
		return Instance;
	}

	ULowEntryJsonArray* ULowEntryJsonArray::CreateFromJsonArrayArray(const TArray<ULowEntryJsonArray*>& Value)
	{
		ULowEntryJsonArray* Instance = NewObject<ULowEntryJsonArray>();
		for(int32 i = 0; i < Value.Num(); i++)
		{
			if(Value[i] != nullptr)
			{
				Instance->Data.Add(MakeShareable(new FJsonValueArray(Value[i]->Data)));
			}
		}
		return Instance;
	}

	ULowEntryJsonArray* ULowEntryJsonArray::CreateFromJsonValueArray(const TArray<ULowEntryJsonValue*>& Value)
	{
		ULowEntryJsonArray* Instance = NewObject<ULowEntryJsonArray>();
		for(int32 i = 0; i < Value.Num(); i++)
		{
			if(Value[i] != nullptr)
			{
				Instance->Data.Add(ULowEntryJsonValue::CloneData(Value[i]->Data));
			}
		}
		return Instance;
	}

	ULowEntryJsonArray* ULowEntryJsonArray::Clone(ULowEntryJsonArray* JsonArray)
	{
		if(JsonArray != nullptr)
		{
			return ULowEntryJsonArray::Create(JsonArray->Data);
		}
		else
		{
			return ULowEntryJsonArray::Create();
		}
	}

	ULowEntryJsonArray* ULowEntryJsonArray::Merge(ULowEntryJsonArray* JsonArray1, ULowEntryJsonArray* JsonArray2)
	{
		ULowEntryJsonArray* Instance = NewObject<ULowEntryJsonArray>();
		if(JsonArray1 != nullptr)
		{
			Instance->Data.Append(JsonArray1->Data);
		}
		if(JsonArray2 != nullptr)
		{
			Instance->Data.Append(JsonArray2->Data);
		}
		return Instance;
	}
// init <<


// json >>
	FString ULowEntryJsonArray::ToJsonString(ULowEntryJsonArray* JsonArray, const bool PrettyPrint)
	{
		if(JsonArray == nullptr)
		{
			return TEXT("");
		}

		FString JsonString;
		if(PrettyPrint)
		{
			TSharedRef<TJsonWriter<TCHAR, TPrettyJsonPrintPolicy<TCHAR>>> JsonWriter = TJsonWriterFactory<TCHAR, TPrettyJsonPrintPolicy<TCHAR>>::Create(&JsonString);
			AddToJson(JsonArray->Data, JsonWriter);
			JsonWriter->Close();
		}
		else
		{
			TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&JsonString);
			AddToJson(JsonArray->Data, JsonWriter);
			JsonWriter->Close();
		}
		return JsonString;
	}

	void ULowEntryJsonArray::AddToJson(const TArray<TSharedPtr<FJsonValue>>& Data, const TSharedRef<TJsonWriter<TCHAR, TPrettyJsonPrintPolicy<TCHAR>>> JsonWriter)
	{
		JsonWriter->WriteArrayStart();
		for(int32 i = 0; i < Data.Num(); i++)
		{
			ULowEntryJsonValue::AddToJson(Data[i], JsonWriter);
		}
		JsonWriter->WriteArrayEnd();
	}
	void ULowEntryJsonArray::AddToJson(const FString& Key, const TArray<TSharedPtr<FJsonValue>>& Data, const TSharedRef<TJsonWriter<TCHAR, TPrettyJsonPrintPolicy<TCHAR>>> JsonWriter)
	{
		JsonWriter->WriteArrayStart(Key);
		for(int32 i = 0; i < Data.Num(); i++)
		{
			ULowEntryJsonValue::AddToJson(Data[i], JsonWriter);
		}
		JsonWriter->WriteArrayEnd();
	}

	void ULowEntryJsonArray::AddToJson(const TArray<TSharedPtr<FJsonValue>>& Data, const TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
	{
		JsonWriter->WriteArrayStart();
		for(int32 i = 0; i < Data.Num(); i++)
		{
			ULowEntryJsonValue::AddToJson(Data[i], JsonWriter);
		}
		JsonWriter->WriteArrayEnd();
	}
	void ULowEntryJsonArray::AddToJson(const FString& Key, const TArray<TSharedPtr<FJsonValue>>& Data, const TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
	{
		JsonWriter->WriteArrayStart(Key);
		for(int32 i = 0; i < Data.Num(); i++)
		{
			ULowEntryJsonValue::AddToJson(Data[i], JsonWriter);
		}
		JsonWriter->WriteArrayEnd();
	}
// json <<


void ULowEntryJsonArray::Clear(ULowEntryJsonArray* JsonArray)
{
	if(JsonArray == nullptr)
	{
		return;
	}
	JsonArray->Data.Empty();
}

void ULowEntryJsonArray::Unset(ULowEntryJsonArray* JsonArray, const int32 Index, const int32 Count)
{
	if(JsonArray == nullptr)
	{
		return;
	}
	int32 Length = JsonArray->Data.Num();
	int32 UseIndex = Index;
	int32 UseCount = Count;
	if(UseIndex >= Length)
	{
		return;
	}
	if(UseCount <= 0)
	{
		return;
	}
	if(UseIndex < 0)
	{
		UseCount += UseIndex;
		UseIndex = 0;
	}
	if((UseIndex == 0) && (UseCount >= Length))
	{
		ULowEntryJsonArray::Clear(JsonArray);
		return;
	}
	UseCount = FMath::Min(Length - UseIndex, UseCount);
	JsonArray->Data.RemoveAt(UseIndex, UseCount);
}

bool ULowEntryJsonArray::IsSet(ULowEntryJsonArray* JsonArray, const int32 Index)
{
	if(JsonArray == nullptr)
	{
		return false;
	}
	return JsonArray->Data.IsValidIndex(Index);
}


void ULowEntryJsonArray::AddNull(ULowEntryJsonArray* JsonArray)
{
	if(JsonArray == nullptr)
	{
		return;
	}
	JsonArray->Data.Add(MakeShareable(new FJsonValueNull()));
}

void ULowEntryJsonArray::AddString(ULowEntryJsonArray* JsonArray, const FString& Value)
{
	if(JsonArray == nullptr)
	{
		return;
	}
	JsonArray->Data.Add(MakeShareable(new FJsonValueString(Value)));
}

void ULowEntryJsonArray::AddInteger(ULowEntryJsonArray* JsonArray, const int32 Value)
{
	if(JsonArray == nullptr)
	{
		return;
	}
	JsonArray->Data.Add(MakeShareable(new FJsonValueNumber(Value)));
}

void ULowEntryJsonArray::AddFloat(ULowEntryJsonArray* JsonArray, const float Value)
{
	if(JsonArray == nullptr)
	{
		return;
	}
	JsonArray->Data.Add(MakeShareable(new FJsonValueNumber(Value)));
}

void ULowEntryJsonArray::AddByte(ULowEntryJsonArray* JsonArray, const uint8 Value)
{
	if(JsonArray == nullptr)
	{
		return;
	}
	JsonArray->Data.Add(MakeShareable(new FJsonValueNumber(Value)));
}

void ULowEntryJsonArray::AddBoolean(ULowEntryJsonArray* JsonArray, const bool Value)
{
	if(JsonArray == nullptr)
	{
		return;
	}
	JsonArray->Data.Add(MakeShareable(new FJsonValueBoolean(Value)));
}

void ULowEntryJsonArray::AddJsonObject(ULowEntryJsonArray* JsonArray, ULowEntryJsonObject* Value)
{
	if(JsonArray == nullptr)
	{
		return;
	}
	if(Value != nullptr)
	{
		JsonArray->Data.Add(MakeShareable(new FJsonValueObject(ULowEntryJsonObject::CloneData(Value->Data))));
	}
	else
	{
		JsonArray->Data.Add(MakeShareable(new FJsonValueNull()));
	}
}

void ULowEntryJsonArray::AddJsonArray(ULowEntryJsonArray* JsonArray, ULowEntryJsonArray* Value)
{
	if(JsonArray == nullptr)
	{
		return;
	}
	if(Value != nullptr)
	{
		JsonArray->Data.Add(MakeShareable(new FJsonValueArray(Value->Data)));
	}
	else
	{
		JsonArray->Data.Add(MakeShareable(new FJsonValueNull()));
	}
}

void ULowEntryJsonArray::AddJsonValue(ULowEntryJsonArray* JsonArray, ULowEntryJsonValue* Value)
{
	if(JsonArray == nullptr)
	{
		return;
	}
	if(Value != nullptr)
	{
		JsonArray->Data.Add(ULowEntryJsonValue::CloneData(Value->Data));
	}
	else
	{
		JsonArray->Data.Add(MakeShareable(new FJsonValueNull()));
	}
}


void ULowEntryJsonArray::SetNull(ULowEntryJsonArray* JsonArray, const int32 Index)
{
	if(JsonArray == nullptr)
	{
		return;
	}
	if(Index < 0)
	{
		return;
	}
	ULowEntryJsonArray::Expand(JsonArray, Index + 1);

	JsonArray->Data[Index] = MakeShareable(new FJsonValueNull());
}

void ULowEntryJsonArray::SetString(ULowEntryJsonArray* JsonArray, const int32 Index, const FString& Value)
{
	if(JsonArray == nullptr)
	{
		return;
	}
	if(Index < 0)
	{
		return;
	}
	ULowEntryJsonArray::Expand(JsonArray, Index + 1);

	JsonArray->Data[Index] = MakeShareable(new FJsonValueString(Value));
}

void ULowEntryJsonArray::SetInteger(ULowEntryJsonArray* JsonArray, const int32 Index, const int32 Value)
{
	if(JsonArray == nullptr)
	{
		return;
	}
	if(Index < 0)
	{
		return;
	}
	ULowEntryJsonArray::Expand(JsonArray, Index + 1);

	JsonArray->Data[Index] = MakeShareable(new FJsonValueNumber(Value));
}

void ULowEntryJsonArray::SetFloat(ULowEntryJsonArray* JsonArray, const int32 Index, const float Value)
{
	if(JsonArray == nullptr)
	{
		return;
	}
	if(Index < 0)
	{
		return;
	}
	ULowEntryJsonArray::Expand(JsonArray, Index + 1);

	JsonArray->Data[Index] = MakeShareable(new FJsonValueNumber(Value));
}

void ULowEntryJsonArray::SetByte(ULowEntryJsonArray* JsonArray, const int32 Index, const uint8 Value)
{
	if(JsonArray == nullptr)
	{
		return;
	}
	if(Index < 0)
	{
		return;
	}
	ULowEntryJsonArray::Expand(JsonArray, Index + 1);

	JsonArray->Data[Index] = MakeShareable(new FJsonValueNumber(Value));
}

void ULowEntryJsonArray::SetBoolean(ULowEntryJsonArray* JsonArray, const int32 Index, const bool Value)
{
	if(JsonArray == nullptr)
	{
		return;
	}
	if(Index < 0)
	{
		return;
	}
	ULowEntryJsonArray::Expand(JsonArray, Index + 1);

	JsonArray->Data[Index] = MakeShareable(new FJsonValueBoolean(Value));
}

void ULowEntryJsonArray::SetJsonObject(ULowEntryJsonArray* JsonArray, const int32 Index, ULowEntryJsonObject*  Value)
{
	if(JsonArray == nullptr)
	{
		return;
	}
	if(Index < 0)
	{
		return;
	}
	ULowEntryJsonArray::Expand(JsonArray, Index + 1);

	if(Value != nullptr)
	{
		JsonArray->Data[Index] = MakeShareable(new FJsonValueObject(ULowEntryJsonObject::CloneData(Value->Data)));
	}
	else
	{
		JsonArray->Data[Index] = MakeShareable(new FJsonValueNull());
	}
}

void ULowEntryJsonArray::SetJsonArray(ULowEntryJsonArray* JsonArray, const int32 Index, ULowEntryJsonArray* Value)
{
	if(JsonArray == nullptr)
	{
		return;
	}
	if(Index < 0)
	{
		return;
	}
	ULowEntryJsonArray::Expand(JsonArray, Index + 1);

	if(Value != nullptr)
	{
		JsonArray->Data[Index] = MakeShareable(new FJsonValueArray(Value->Data));
	}
	else
	{
		JsonArray->Data[Index] = MakeShareable(new FJsonValueNull());
	}
}

void ULowEntryJsonArray::SetJsonValue(ULowEntryJsonArray* JsonArray, const int32 Index, ULowEntryJsonValue* Value)
{
	if(JsonArray == nullptr)
	{
		return;
	}
	if(Index < 0)
	{
		return;
	}
	ULowEntryJsonArray::Expand(JsonArray, Index + 1);

	if(Value != nullptr)
	{
		JsonArray->Data[Index] = ULowEntryJsonValue::CloneData(Value->Data);
	}
	else
	{
		JsonArray->Data[Index] = MakeShareable(new FJsonValueNull());
	}
}


void ULowEntryJsonArray::InsertNull(ULowEntryJsonArray* JsonArray, const int32 Index)
{
	if(JsonArray == nullptr)
	{
		return;
	}
	if(Index < 0)
	{
		return;
	}
	ULowEntryJsonArray::Expand(JsonArray, Index);

	JsonArray->Data.Insert(MakeShareable(new FJsonValueNull()), Index);
}

void ULowEntryJsonArray::InsertString(ULowEntryJsonArray* JsonArray, const int32 Index, const FString& Value)
{
	if(JsonArray == nullptr)
	{
		return;
	}
	if(Index < 0)
	{
		return;
	}
	ULowEntryJsonArray::Expand(JsonArray, Index);

	JsonArray->Data.Insert(MakeShareable(new FJsonValueString(Value)), Index);
}

void ULowEntryJsonArray::InsertInteger(ULowEntryJsonArray* JsonArray, const int32 Index, const int32 Value)
{
	if(JsonArray == nullptr)
	{
		return;
	}
	if(Index < 0)
	{
		return;
	}
	ULowEntryJsonArray::Expand(JsonArray, Index);

	JsonArray->Data.Insert(MakeShareable(new FJsonValueNumber(Value)), Index);
}

void ULowEntryJsonArray::InsertFloat(ULowEntryJsonArray* JsonArray, const int32 Index, const float Value)
{
	if(JsonArray == nullptr)
	{
		return;
	}
	if(Index < 0)
	{
		return;
	}
	ULowEntryJsonArray::Expand(JsonArray, Index);

	JsonArray->Data.Insert(MakeShareable(new FJsonValueNumber(Value)), Index);
}

void ULowEntryJsonArray::InsertByte(ULowEntryJsonArray* JsonArray, const int32 Index, const uint8 Value)
{
	if(JsonArray == nullptr)
	{
		return;
	}
	if(Index < 0)
	{
		return;
	}
	ULowEntryJsonArray::Expand(JsonArray, Index);

	JsonArray->Data.Insert(MakeShareable(new FJsonValueNumber(Value)), Index);
}

void ULowEntryJsonArray::InsertBoolean(ULowEntryJsonArray* JsonArray, const int32 Index, const bool Value)
{
	if(JsonArray == nullptr)
	{
		return;
	}
	if(Index < 0)
	{
		return;
	}
	ULowEntryJsonArray::Expand(JsonArray, Index);

	JsonArray->Data.Insert(MakeShareable(new FJsonValueBoolean(Value)), Index);
}

void ULowEntryJsonArray::InsertJsonObject(ULowEntryJsonArray* JsonArray, const int32 Index, ULowEntryJsonObject* Value)
{
	if(JsonArray == nullptr)
	{
		return;
	}
	if(Index < 0)
	{
		return;
	}
	ULowEntryJsonArray::Expand(JsonArray, Index);

	if(Value != nullptr)
	{
		JsonArray->Data.Insert(MakeShareable(new FJsonValueObject(ULowEntryJsonObject::CloneData(Value->Data))), Index);
	}
	else
	{
		JsonArray->Data.Insert(MakeShareable(new FJsonValueNull()), Index);
	}
}

void ULowEntryJsonArray::InsertJsonArray(ULowEntryJsonArray* JsonArray, const int32 Index, ULowEntryJsonArray* Value)
{
	if(JsonArray == nullptr)
	{
		return;
	}
	if(Index < 0)
	{
		return;
	}
	ULowEntryJsonArray::Expand(JsonArray, Index);

	if(Value != nullptr)
	{
		JsonArray->Data.Insert(MakeShareable(new FJsonValueArray(Value->Data)), Index);
	}
	else
	{
		JsonArray->Data.Insert(MakeShareable(new FJsonValueNull()), Index);
	}
}

void ULowEntryJsonArray::InsertJsonValue(ULowEntryJsonArray* JsonArray, const int32 Index, ULowEntryJsonValue* Value)
{
	if(JsonArray == nullptr)
	{
		return;
	}
	if(Index < 0)
	{
		return;
	}
	ULowEntryJsonArray::Expand(JsonArray, Index);

	if(Value != nullptr)
	{
		JsonArray->Data.Insert(ULowEntryJsonValue::CloneData(Value->Data), Index);
	}
	else
	{
		JsonArray->Data.Insert(MakeShareable(new FJsonValueNull()), Index);
	}
}


void ULowEntryJsonArray::Get(ULowEntryJsonArray* JsonArray, const int32 Index, ELowEntryJsonType& Branch, FString& ReturnStringValue, int32& ReturnIntegerValue, float& ReturnFloatValue, uint8& ReturnByteValue, bool& ReturnBooleanValue, ULowEntryJsonObject*& ReturnJsonObjectValue, ULowEntryJsonArray*& ReturnJsonArrayValue)
{
	if(JsonArray == nullptr)
	{
		Branch = ELowEntryJsonType::None;
		return;
	}
	if(!JsonArray->Data.IsValidIndex(Index))
	{
		Branch = ELowEntryJsonType::None;
		return;
	}
	TSharedPtr<FJsonValue> Field = JsonArray->Data[Index];
	if(!Field.IsValid())
	{
		Branch = ELowEntryJsonType::None;
		return;
	}
	TSharedPtr<FJsonValue> Data = Field;
	if(Data->Type == EJson::None)
	{
		Branch = ELowEntryJsonType::None;
	}
	else if(Data->Type == EJson::Null)
	{
		Branch = ELowEntryJsonType::Null;
	}
	else if(Data->Type == EJson::String)
	{
		ReturnStringValue = Data->AsString();
		Branch = ELowEntryJsonType::String;
	}
	else if(Data->Type == EJson::Number)
	{
		ReturnFloatValue = Data->AsNumber();
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
	else if(Data->Type == EJson::Boolean)
	{
		ReturnBooleanValue = Data->AsBool();
		Branch = ELowEntryJsonType::Boolean;
	}
	else if(Data->Type == EJson::Object)
	{
		ReturnJsonObjectValue = ULowEntryJsonObject::Create(Data->AsObject());
		Branch = ELowEntryJsonType::JsonObject;
	}
	else if(Data->Type == EJson::Array)
	{
		ReturnJsonArrayValue = ULowEntryJsonArray::Create(Data->AsArray());
		Branch = ELowEntryJsonType::JsonArray;
	}
	else
	{
		Branch = ELowEntryJsonType::None;
	}
}

void ULowEntryJsonArray::GetNull(ULowEntryJsonArray* JsonArray, const int32 Index, ELowEntryJsonValueAndTypeFound& Branch)
{
	if(JsonArray == nullptr)
	{
		Branch = ELowEntryJsonValueAndTypeFound::NotFound;
		return;
	}
	if(!JsonArray->Data.IsValidIndex(Index))
	{
		Branch = ELowEntryJsonValueAndTypeFound::NotFound;
		return;
	}
	TSharedPtr<FJsonValue> Field = JsonArray->Data[Index];
	if(!Field.IsValid())
	{
		Branch = ELowEntryJsonValueAndTypeFound::NotFound;
		return;
	}
	TSharedPtr<FJsonValue> Data = Field;
	if(Data->Type == EJson::Null)
	{
		Branch = ELowEntryJsonValueAndTypeFound::Success;
		return;
	}
	Branch = ELowEntryJsonValueAndTypeFound::WrongType;
	return;
}

FString ULowEntryJsonArray::GetString(ULowEntryJsonArray* JsonArray, const int32 Index, ELowEntryJsonValueAndTypeFound& Branch)
{
	if(JsonArray == nullptr)
	{
		Branch = ELowEntryJsonValueAndTypeFound::NotFound;
		return TEXT("");
	}
	if(!JsonArray->Data.IsValidIndex(Index))
	{
		Branch = ELowEntryJsonValueAndTypeFound::NotFound;
		return TEXT("");
	}
	TSharedPtr<FJsonValue> Field = JsonArray->Data[Index];
	if(!Field.IsValid())
	{
		Branch = ELowEntryJsonValueAndTypeFound::NotFound;
		return TEXT("");
	}
	TSharedPtr<FJsonValue> Data = Field;
	if(Data->Type == EJson::String)
	{
		Branch = ELowEntryJsonValueAndTypeFound::Success;
		return Data->AsString();
	}
	Branch = ELowEntryJsonValueAndTypeFound::WrongType;
	return TEXT("");
}

int32 ULowEntryJsonArray::GetInteger(ULowEntryJsonArray* JsonArray, const int32 Index, ELowEntryJsonValueAndTypeFound& Branch)
{
	if(JsonArray == nullptr)
	{
		Branch = ELowEntryJsonValueAndTypeFound::NotFound;
		return 0;
	}
	if(!JsonArray->Data.IsValidIndex(Index))
	{
		Branch = ELowEntryJsonValueAndTypeFound::NotFound;
		return 0;
	}
	TSharedPtr<FJsonValue> Field = JsonArray->Data[Index];
	if(!Field.IsValid())
	{
		Branch = ELowEntryJsonValueAndTypeFound::NotFound;
		return 0;
	}
	TSharedPtr<FJsonValue> Data = Field;
	if(Data->Type == EJson::Number)
	{
		float ReturnFloatValue = Data->AsNumber();
		float Remainder = FMath::Fmod(ReturnFloatValue, 1);
		if((Remainder <= 0.0001) && (Remainder >= -0.0001))
		{
			Branch = ELowEntryJsonValueAndTypeFound::Success;
			return FMath::RoundToInt(ReturnFloatValue);
		}
	}
	Branch = ELowEntryJsonValueAndTypeFound::WrongType;
	return 0;
}

float ULowEntryJsonArray::GetFloat(ULowEntryJsonArray* JsonArray, const int32 Index, ELowEntryJsonValueAndTypeFound& Branch)
{
	if(JsonArray == nullptr)
	{
		Branch = ELowEntryJsonValueAndTypeFound::NotFound;
		return 0;
	}
	if(!JsonArray->Data.IsValidIndex(Index))
	{
		Branch = ELowEntryJsonValueAndTypeFound::NotFound;
		return 0;
	}
	TSharedPtr<FJsonValue> Field = JsonArray->Data[Index];
	if(!Field.IsValid())
	{
		Branch = ELowEntryJsonValueAndTypeFound::NotFound;
		return 0;
	}
	TSharedPtr<FJsonValue> Data = Field;
	if(Data->Type == EJson::Number)
	{
		Branch = ELowEntryJsonValueAndTypeFound::Success;
		return Data->AsNumber();
	}
	Branch = ELowEntryJsonValueAndTypeFound::WrongType;
	return 0;
}

uint8 ULowEntryJsonArray::GetByte(ULowEntryJsonArray* JsonArray, const int32 Index, ELowEntryJsonValueAndTypeFound& Branch)
{
	if(JsonArray == nullptr)
	{
		Branch = ELowEntryJsonValueAndTypeFound::NotFound;
		return 0;
	}
	if(!JsonArray->Data.IsValidIndex(Index))
	{
		Branch = ELowEntryJsonValueAndTypeFound::NotFound;
		return 0;
	}
	TSharedPtr<FJsonValue> Field = JsonArray->Data[Index];
	if(!Field.IsValid())
	{
		Branch = ELowEntryJsonValueAndTypeFound::NotFound;
		return 0;
	}
	TSharedPtr<FJsonValue> Data = Field;
	if(Data->Type == EJson::Number)
	{
		float ReturnFloatValue = Data->AsNumber();
		float Remainder = FMath::Fmod(ReturnFloatValue, 1);
		if((Remainder <= 0.0001) && (Remainder >= -0.0001))
		{
			int32 ReturnIntegerValue = FMath::RoundToInt(ReturnFloatValue);
			if((ReturnIntegerValue >= 0) && (ReturnIntegerValue <= 255))
			{
				Branch = ELowEntryJsonValueAndTypeFound::Success;
				return (uint8) ReturnIntegerValue;
			}
		}
	}
	Branch = ELowEntryJsonValueAndTypeFound::WrongType;
	return 0;
}

bool ULowEntryJsonArray::GetBoolean(ULowEntryJsonArray* JsonArray, const int32 Index, ELowEntryJsonValueAndTypeFound& Branch)
{
	if(JsonArray == nullptr)
	{
		Branch = ELowEntryJsonValueAndTypeFound::NotFound;
		return false;
	}
	if(!JsonArray->Data.IsValidIndex(Index))
	{
		Branch = ELowEntryJsonValueAndTypeFound::NotFound;
		return false;
	}
	TSharedPtr<FJsonValue> Field = JsonArray->Data[Index];
	if(!Field.IsValid())
	{
		Branch = ELowEntryJsonValueAndTypeFound::NotFound;
		return false;
	}
	TSharedPtr<FJsonValue> Data = Field;
	if(Data->Type == EJson::Boolean)
	{
		Branch = ELowEntryJsonValueAndTypeFound::Success;
		return Data->AsBool();
	}
	Branch = ELowEntryJsonValueAndTypeFound::WrongType;
	return false;
}

ULowEntryJsonObject* ULowEntryJsonArray::GetJsonObject(ULowEntryJsonArray* JsonArray, const int32 Index, ELowEntryJsonValueAndTypeFound& Branch)
{
	if(JsonArray == nullptr)
	{
		Branch = ELowEntryJsonValueAndTypeFound::NotFound;
		return nullptr;
	}
	if(!JsonArray->Data.IsValidIndex(Index))
	{
		Branch = ELowEntryJsonValueAndTypeFound::NotFound;
		return nullptr;
	}
	TSharedPtr<FJsonValue> Field = JsonArray->Data[Index];
	if(!Field.IsValid())
	{
		Branch = ELowEntryJsonValueAndTypeFound::NotFound;
		return nullptr;
	}
	TSharedPtr<FJsonValue> Data = Field;
	if(Data->Type == EJson::Object)
	{
		Branch = ELowEntryJsonValueAndTypeFound::Success;
		return ULowEntryJsonObject::Create(Data->AsObject());
	}
	Branch = ELowEntryJsonValueAndTypeFound::WrongType;
	return nullptr;
}

ULowEntryJsonArray* ULowEntryJsonArray::GetJsonArray(ULowEntryJsonArray* JsonArray, const int32 Index, ELowEntryJsonValueAndTypeFound& Branch)
{
	if(JsonArray == nullptr)
	{
		Branch = ELowEntryJsonValueAndTypeFound::NotFound;
		return nullptr;
	}
	if(!JsonArray->Data.IsValidIndex(Index))
	{
		Branch = ELowEntryJsonValueAndTypeFound::NotFound;
		return nullptr;
	}
	TSharedPtr<FJsonValue> Field = JsonArray->Data[Index];
	if(!Field.IsValid())
	{
		Branch = ELowEntryJsonValueAndTypeFound::NotFound;
		return nullptr;
	}
	TSharedPtr<FJsonValue> Data = Field;
	if(Data->Type == EJson::Array)
	{
		Branch = ELowEntryJsonValueAndTypeFound::Success;
		return ULowEntryJsonArray::Create(Data->AsArray());
	}
	Branch = ELowEntryJsonValueAndTypeFound::WrongType;
	return nullptr;
}

ULowEntryJsonValue* ULowEntryJsonArray::GetJsonValue(ULowEntryJsonArray* JsonArray, const int32 Index, ELowEntryJsonValueFound& Branch)
{
	if(JsonArray == nullptr)
	{
		Branch = ELowEntryJsonValueFound::NotFound;
		return nullptr;
	}
	if(!JsonArray->Data.IsValidIndex(Index))
	{
		Branch = ELowEntryJsonValueFound::NotFound;
		return nullptr;
	}
	TSharedPtr<FJsonValue> Field = JsonArray->Data[Index];
	if(!Field.IsValid())
	{
		Branch = ELowEntryJsonValueFound::NotFound;
		return nullptr;
	}
	Branch = ELowEntryJsonValueFound::Found;
	TSharedPtr<FJsonValue> Data = Field;
	return ULowEntryJsonValue::Create(Data);
}


FString ULowEntryJsonArray::GetAsStringOrDefault(ULowEntryJsonArray* JsonArray, const int32 Index, const FString& Default)
{
	if(JsonArray == nullptr)
	{
		return Default;
	}
	if(!JsonArray->Data.IsValidIndex(Index))
	{
		return Default;
	}
	TSharedPtr<FJsonValue> Field = JsonArray->Data[Index];

	FString Value;
	if(Field->TryGetString(Value))
	{
		return Value;
	}
	return Default;
}

int32 ULowEntryJsonArray::GetAsIntegerOrDefault(ULowEntryJsonArray* JsonArray, const int32 Index, const int32 Default)
{
	if(JsonArray == nullptr)
	{
		return Default;
	}
	if(!JsonArray->Data.IsValidIndex(Index))
	{
		return Default;
	}
	TSharedPtr<FJsonValue> Field = JsonArray->Data[Index];

	int32 Value;
	if(Field->TryGetNumber(Value))
	{
		return Value;
	}
	return Default;
}

float ULowEntryJsonArray::GetAsFloatOrDefault(ULowEntryJsonArray* JsonArray, const int32 Index, const float Default)
{
	if(JsonArray == nullptr)
	{
		return Default;
	}
	if(!JsonArray->Data.IsValidIndex(Index))
	{
		return Default;
	}
	TSharedPtr<FJsonValue> Field = JsonArray->Data[Index];

	double Value;
	if(Field->TryGetNumber(Value))
	{
		return (float) Value;
	}
	return Default;
}

uint8 ULowEntryJsonArray::GetAsByteOrDefault(ULowEntryJsonArray* JsonArray, const int32 Index, const uint8 Default)
{
	if(JsonArray == nullptr)
	{
		return Default;
	}
	if(!JsonArray->Data.IsValidIndex(Index))
	{
		return Default;
	}
	TSharedPtr<FJsonValue> Field = JsonArray->Data[Index];

	int32 Value;
	if(Field->TryGetNumber(Value))
	{
		if((Value >= 0) && (Value <= 255))
		{
			return (uint8) Value;
		}
	}
	return Default;
}

bool ULowEntryJsonArray::GetAsBooleanOrDefault(ULowEntryJsonArray* JsonArray, const int32 Index, const bool Default)
{
	if(JsonArray == nullptr)
	{
		return Default;
	}
	if(!JsonArray->Data.IsValidIndex(Index))
	{
		return Default;
	}
	TSharedPtr<FJsonValue> Field = JsonArray->Data[Index];

	bool Value;
	if(Field->TryGetBool(Value))
	{
		return Value;
	}
	return Default;
}

ULowEntryJsonObject* ULowEntryJsonArray::GetAsJsonObjectOrDefault(ULowEntryJsonArray* JsonArray, const int32 Index, ULowEntryJsonObject* Default)
{
	//if(Default == nullptr)
	//{
	//	Default = ULowEntryJsonObject::Create();
	//}

	if(JsonArray == nullptr)
	{
		return Default;
	}
	if(!JsonArray->Data.IsValidIndex(Index))
	{
		return Default;
	}
	TSharedPtr<FJsonValue> Field = JsonArray->Data[Index];

	const TSharedPtr<FJsonObject>* Value;
	if(Field->TryGetObject(Value))
	{
		return ULowEntryJsonObject::Create(*Value);
	}
	return Default;
}

ULowEntryJsonArray* ULowEntryJsonArray::GetAsJsonArrayOrDefault(ULowEntryJsonArray* JsonArray, const int32 Index, ULowEntryJsonArray* Default)
{
	//if(Default == nullptr)
	//{
	//	Default = ULowEntryJsonArray::Create();
	//}

	if(JsonArray == nullptr)
	{
		return Default;
	}
	if(!JsonArray->Data.IsValidIndex(Index))
	{
		return Default;
	}
	TSharedPtr<FJsonValue> Field = JsonArray->Data[Index];

	const TArray<TSharedPtr<FJsonValue>>* Value;
	if(Field->TryGetArray(Value))
	{
		return ULowEntryJsonArray::Create(*Value);
	}
	return Default;
}

ULowEntryJsonValue* ULowEntryJsonArray::GetAsJsonValueOrDefault(ULowEntryJsonArray* JsonArray, const int32 Index, ULowEntryJsonValue* Default)
{
	//if(Default == nullptr)
	//{
	//	Default = ULowEntryJsonArray::Create();
	//}

	if(JsonArray == nullptr)
	{
		return Default;
	}
	if(!JsonArray->Data.IsValidIndex(Index))
	{
		return Default;
	}
	TSharedPtr<FJsonValue> Field = JsonArray->Data[Index];

	return ULowEntryJsonValue::Create(Field);
}


int32 ULowEntryJsonArray::Length(ULowEntryJsonArray* JsonArray)
{
	if(JsonArray != nullptr)
	{
		return JsonArray->Data.Num();
	}
	return 0;
}


void ULowEntryJsonArray::Resize(ULowEntryJsonArray* JsonArray, const int32 Size)
{
	if(JsonArray == nullptr)
	{
		return;
	}
	ULowEntryJsonArray::Expand(JsonArray, Size);
	ULowEntryJsonArray::Shorten(JsonArray, Size);
}

void ULowEntryJsonArray::Expand(ULowEntryJsonArray* JsonArray, const int32 Size)
{
	if(JsonArray == nullptr)
	{
		return;
	}
	while(JsonArray->Data.Num() < Size)
	{
		ULowEntryJsonArray::AddNull(JsonArray);
	}
}

void ULowEntryJsonArray::Shorten(ULowEntryJsonArray* JsonArray, const int32 Size)
{
	if(JsonArray == nullptr)
	{
		return;
	}
	if(Size <= 0)
	{
		ULowEntryJsonArray::Clear(JsonArray);
	}
	else
	{
		ULowEntryJsonArray::Unset(JsonArray, Size - 1, JsonArray->Data.Num() - Size);
	}
}


void ULowEntryJsonArray::Append(ULowEntryJsonArray* JsonArray, ULowEntryJsonArray* AppendJsonArray)
{
	if(JsonArray == nullptr)
	{
		return;
	}
	if(AppendJsonArray != nullptr)
	{
		JsonArray->Data.Append(JsonArray->Data);
	}
}
