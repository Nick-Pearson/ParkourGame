#include "LowEntryJsonObject.h"
#include "LowEntryJsonArray.h"
#include "LowEntryJsonValue.h"
#include "LowEntryJsonObjectEntry.h"


// init >>
	ULowEntryJsonObject::ULowEntryJsonObject(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
	{
		Data = MakeShareable(new FJsonObject());
	}

	ULowEntryJsonObject* ULowEntryJsonObject::Create()
	{
		return NewObject<ULowEntryJsonObject>();
	}

	ULowEntryJsonObject* ULowEntryJsonObject::Create(const TSharedPtr<FJsonObject> Value)
	{
		ULowEntryJsonObject* Instance = NewObject<ULowEntryJsonObject>();
		if(Value.IsValid())
		{
			Instance->Data = ULowEntryJsonObject::CloneData(Value);
		}
		else
		{
			Instance->Data = MakeShareable(new FJsonObject());
		}
		return Instance;
	}

	ULowEntryJsonObject* ULowEntryJsonObject::Create(const TArray<ULowEntryJsonObjectEntry*>& Value)
	{
		ULowEntryJsonObject* Instance = NewObject<ULowEntryJsonObject>();
		for(int32 i = 0; i < Value.Num(); i++)
		{
			ULowEntryJsonObjectEntry* Item = Value[i];
			if((Item != nullptr) && (Item->Value != nullptr) && (Item->Value->Data.IsValid()))
			{
				Instance->Data->SetField(Item->Key, Item->Value->Data);
			}
		}
		return Instance;
	}

	ULowEntryJsonObject* ULowEntryJsonObject::Clone(ULowEntryJsonObject* JsonObject)
	{
		if(JsonObject != nullptr)
		{
			return ULowEntryJsonObject::Create(JsonObject->Data);
		}
		else
		{
			return ULowEntryJsonObject::Create();
		}
	}

	ULowEntryJsonObject* ULowEntryJsonObject::Merge(ULowEntryJsonObject* JsonObject1, ULowEntryJsonObject* JsonObject2)
	{
		ULowEntryJsonObject* Instance = NewObject<ULowEntryJsonObject>();
		Instance->Data = MakeShareable(new FJsonObject());
		if(JsonObject1 != nullptr)
		{
			Instance->Data->Values.Append(JsonObject1->Data->Values);
		}
		if(JsonObject2 != nullptr)
		{
			Instance->Data->Values.Append(JsonObject2->Data->Values);
		}
		return Instance;
	}

	TSharedPtr<FJsonObject> ULowEntryJsonObject::CloneData(const TSharedPtr<FJsonObject> Data)
	{
		TSharedPtr<FJsonObject> NewData = MakeShareable(new FJsonObject());
		NewData->Values.Append(Data->Values);
		return NewData;
	}
// init <<


// json >>
	FString ULowEntryJsonObject::ToJsonString(ULowEntryJsonObject* JsonObject, const bool PrettyPrint)
	{
		if(JsonObject == nullptr)
		{
			return TEXT("");
		}

		FString JsonString;
		if(PrettyPrint)
		{
			TSharedRef<TJsonWriter<TCHAR, TPrettyJsonPrintPolicy<TCHAR>>> JsonWriter = TJsonWriterFactory<TCHAR, TPrettyJsonPrintPolicy<TCHAR>>::Create(&JsonString);
			AddToJson(JsonObject->Data, JsonWriter);
			JsonWriter->Close();
		}
		else
		{
			TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&JsonString);
			AddToJson(JsonObject->Data, JsonWriter);
			JsonWriter->Close();
		}
		return JsonString;
	}

	void ULowEntryJsonObject::AddToJson(const TSharedPtr<FJsonObject> Data, const TSharedRef<TJsonWriter<TCHAR, TPrettyJsonPrintPolicy<TCHAR>>> JsonWriter)
	{
		if(!Data.IsValid())
		{
			JsonWriter->WriteNull();
			return;
		}

		JsonWriter->WriteObjectStart();
		TMap<FString, TSharedPtr<FJsonValue>>::TIterator Iterator = Data->Values.CreateIterator();
		while(Iterator)
		{
			ULowEntryJsonValue::AddToJson(Iterator.Key(), Iterator.Value(), JsonWriter);
			++Iterator;
		}
		JsonWriter->WriteObjectEnd();
	}
	void ULowEntryJsonObject::AddToJson(const FString& Key, const TSharedPtr<FJsonObject> Data, const TSharedRef<TJsonWriter<TCHAR, TPrettyJsonPrintPolicy<TCHAR>>> JsonWriter)
	{
		if(!Data.IsValid())
		{
			JsonWriter->WriteNull();
			return;
		}

		JsonWriter->WriteObjectStart(Key);
		TMap<FString, TSharedPtr<FJsonValue>>::TIterator Iterator = Data->Values.CreateIterator();
		while(Iterator)
		{
			ULowEntryJsonValue::AddToJson(Iterator.Key(), Iterator.Value(), JsonWriter);
			++Iterator;
		}
		JsonWriter->WriteObjectEnd();
	}

	void ULowEntryJsonObject::AddToJson(const TSharedPtr<FJsonObject> Data, const TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
	{
		if(!Data.IsValid())
		{
			JsonWriter->WriteNull();
			return;
		}

		JsonWriter->WriteObjectStart();
		TMap<FString, TSharedPtr<FJsonValue>>::TIterator Iterator = Data->Values.CreateIterator();
		while(Iterator)
		{
			ULowEntryJsonValue::AddToJson(Iterator.Key(), Iterator.Value(), JsonWriter);
			++Iterator;
		}
		JsonWriter->WriteObjectEnd();
	}
	void ULowEntryJsonObject::AddToJson(const FString& Key, const TSharedPtr<FJsonObject> Data, const TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
	{
		if(!Data.IsValid())
		{
			JsonWriter->WriteNull();
			return;
		}

		JsonWriter->WriteObjectStart(Key);
		TMap<FString, TSharedPtr<FJsonValue>>::TIterator Iterator = Data->Values.CreateIterator();
		while(Iterator)
		{
			ULowEntryJsonValue::AddToJson(Iterator.Key(), Iterator.Value(), JsonWriter);
			++Iterator;
		}
		JsonWriter->WriteObjectEnd();
	}
// json <<


void ULowEntryJsonObject::Clear(ULowEntryJsonObject* JsonObject)
{
	if(JsonObject == nullptr)
	{
		return;
	}
	JsonObject->Data = MakeShareable(new FJsonObject());
}

void ULowEntryJsonObject::Unset(ULowEntryJsonObject* JsonObject, const FString& Key)
{
	if(JsonObject == nullptr)
	{
		return;
	}
	JsonObject->Data->RemoveField(Key);
}

bool ULowEntryJsonObject::IsSet(ULowEntryJsonObject* JsonObject, const FString& Key)
{
	if(JsonObject == nullptr)
	{
		return false;
	}
	return JsonObject->Data->HasField(Key);
}


void ULowEntryJsonObject::SetNull(ULowEntryJsonObject* JsonObject, const FString& Key)
{
	if(JsonObject == nullptr)
	{
		return;
	}
	JsonObject->Data->SetField(Key, MakeShareable(new FJsonValueNull()));
}

void ULowEntryJsonObject::SetString(ULowEntryJsonObject* JsonObject, const FString& Key, const FString& Value)
{
	if(JsonObject == nullptr)
	{
		return;
	}
	JsonObject->Data->SetStringField(Key, Value);
}

void ULowEntryJsonObject::SetInteger(ULowEntryJsonObject* JsonObject, const FString& Key, const int32 Value)
{
	if(JsonObject == nullptr)
	{
		return;
	}
	JsonObject->Data->SetNumberField(Key, Value);
}

void ULowEntryJsonObject::SetFloat(ULowEntryJsonObject* JsonObject, const FString& Key, const float Value)
{
	if(JsonObject == nullptr)
	{
		return;
	}
	JsonObject->Data->SetNumberField(Key, Value);
}

void ULowEntryJsonObject::SetByte(ULowEntryJsonObject* JsonObject, const FString& Key, const uint8 Value)
{
	if(JsonObject == nullptr)
	{
		return;
	}
	JsonObject->Data->SetNumberField(Key, Value);
}

void ULowEntryJsonObject::SetBoolean(ULowEntryJsonObject* JsonObject, const FString& Key, const bool Value)
{
	if(JsonObject == nullptr)
	{
		return;
	}
	JsonObject->Data->SetBoolField(Key, Value);
}

void ULowEntryJsonObject::SetJsonObject(ULowEntryJsonObject* JsonObject, const FString& Key, ULowEntryJsonObject* Value)
{
	if(JsonObject == nullptr)
	{
		return;
	}
	if(Value != nullptr)
	{
		JsonObject->Data->SetObjectField(Key, ULowEntryJsonObject::CloneData(Value->Data));
	}
	else
	{
		JsonObject->Data->SetField(Key, MakeShareable(new FJsonValueNull()));
	}
}

void ULowEntryJsonObject::SetJsonArray(ULowEntryJsonObject* JsonObject, const FString& Key, ULowEntryJsonArray* Value)
{
	if(JsonObject == nullptr)
	{
		return;
	}
	if(Value != nullptr)
	{
		JsonObject->Data->SetArrayField(Key, Value->Data);
	}
	else
	{
		JsonObject->Data->SetField(Key, MakeShareable(new FJsonValueNull()));
	}
}

void ULowEntryJsonObject::SetJsonValue(ULowEntryJsonObject* JsonObject, const FString& Key, ULowEntryJsonValue* Value)
{
	if(JsonObject == nullptr)
	{
		return;
	}
	if(Value != nullptr)
	{
		JsonObject->Data->SetField(Key, ULowEntryJsonValue::CloneData(Value->Data));
	}
	else
	{
		JsonObject->Data->SetField(Key, MakeShareable(new FJsonValueNull()));
	}
}


void ULowEntryJsonObject::Get(ULowEntryJsonObject* JsonObject, const FString& Key, ELowEntryJsonType& Branch, FString& ReturnStringValue, int32& ReturnIntegerValue, float& ReturnFloatValue, uint8& ReturnByteValue, bool& ReturnBooleanValue, ULowEntryJsonObject*& ReturnJsonObjectValue, ULowEntryJsonArray*& ReturnJsonArrayValue)
{
	if(JsonObject == nullptr)
	{
		Branch = ELowEntryJsonType::None;
		return;
	}
	TSharedPtr<FJsonValue>* Field = JsonObject->Data->Values.Find(Key);
	if((Field == nullptr) || !Field->IsValid())
	{
		Branch = ELowEntryJsonType::None;
		return;
	}
	TSharedPtr<FJsonValue> Data = *Field;
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

void ULowEntryJsonObject::GetNull(ULowEntryJsonObject* JsonObject, const FString& Key, ELowEntryJsonValueAndTypeFound& Branch)
{
	if(JsonObject == nullptr)
	{
		Branch = ELowEntryJsonValueAndTypeFound::NotFound;
		return;
	}
	TSharedPtr<FJsonValue>* Field = JsonObject->Data->Values.Find(Key);
	if((Field == nullptr) || !Field->IsValid())
	{
		Branch = ELowEntryJsonValueAndTypeFound::NotFound;
		return;
	}
	TSharedPtr<FJsonValue> Data = *Field;
	if(Data->Type == EJson::Null)
	{
		Branch = ELowEntryJsonValueAndTypeFound::Success;
		return;
	}
	Branch = ELowEntryJsonValueAndTypeFound::WrongType;
	return;
}

FString ULowEntryJsonObject::GetString(ULowEntryJsonObject* JsonObject, const FString& Key, ELowEntryJsonValueAndTypeFound& Branch)
{
	if(JsonObject == nullptr)
	{
		Branch = ELowEntryJsonValueAndTypeFound::NotFound;
		return TEXT("");
	}
	TSharedPtr<FJsonValue>* Field = JsonObject->Data->Values.Find(Key);
	if((Field == nullptr) || !Field->IsValid())
	{
		Branch = ELowEntryJsonValueAndTypeFound::NotFound;
		return TEXT("");
	}
	TSharedPtr<FJsonValue> Data = *Field;
	if(Data->Type == EJson::String)
	{
		Branch = ELowEntryJsonValueAndTypeFound::Success;
		return Data->AsString();
	}
	Branch = ELowEntryJsonValueAndTypeFound::WrongType;
	return TEXT("");
}

int32 ULowEntryJsonObject::GetInteger(ULowEntryJsonObject* JsonObject, const FString& Key, ELowEntryJsonValueAndTypeFound& Branch)
{
	if(JsonObject == nullptr)
	{
		Branch = ELowEntryJsonValueAndTypeFound::NotFound;
		return 0;
	}
	TSharedPtr<FJsonValue>* Field = JsonObject->Data->Values.Find(Key);
	if((Field == nullptr) || !Field->IsValid())
	{
		Branch = ELowEntryJsonValueAndTypeFound::NotFound;
		return 0;
	}
	TSharedPtr<FJsonValue> Data = *Field;
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

float ULowEntryJsonObject::GetFloat(ULowEntryJsonObject* JsonObject, const FString& Key, ELowEntryJsonValueAndTypeFound& Branch)
{
	if(JsonObject == nullptr)
	{
		Branch = ELowEntryJsonValueAndTypeFound::NotFound;
		return 0;
	}
	TSharedPtr<FJsonValue>* Field = JsonObject->Data->Values.Find(Key);
	if((Field == nullptr) || !Field->IsValid())
	{
		Branch = ELowEntryJsonValueAndTypeFound::NotFound;
		return 0;
	}
	TSharedPtr<FJsonValue> Data = *Field;
	if(Data->Type == EJson::Number)
	{
		Branch = ELowEntryJsonValueAndTypeFound::Success;
		return Data->AsNumber();
	}
	Branch = ELowEntryJsonValueAndTypeFound::WrongType;
	return 0;
}

uint8 ULowEntryJsonObject::GetByte(ULowEntryJsonObject* JsonObject, const FString& Key, ELowEntryJsonValueAndTypeFound& Branch)
{
	if(JsonObject == nullptr)
	{
		Branch = ELowEntryJsonValueAndTypeFound::NotFound;
		return 0;
	}
	TSharedPtr<FJsonValue>* Field = JsonObject->Data->Values.Find(Key);
	if((Field == nullptr) || !Field->IsValid())
	{
		Branch = ELowEntryJsonValueAndTypeFound::NotFound;
		return 0;
	}
	TSharedPtr<FJsonValue> Data = *Field;
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

bool ULowEntryJsonObject::GetBoolean(ULowEntryJsonObject* JsonObject, const FString& Key, ELowEntryJsonValueAndTypeFound& Branch)
{
	if(JsonObject == nullptr)
	{
		Branch = ELowEntryJsonValueAndTypeFound::NotFound;
		return false;
	}
	TSharedPtr<FJsonValue>* Field = JsonObject->Data->Values.Find(Key);
	if((Field == nullptr) || !Field->IsValid())
	{
		Branch = ELowEntryJsonValueAndTypeFound::NotFound;
		return false;
	}
	TSharedPtr<FJsonValue> Data = *Field;
	if(Data->Type == EJson::Boolean)
	{
		Branch = ELowEntryJsonValueAndTypeFound::Success;
		return Data->AsBool();
	}
	Branch = ELowEntryJsonValueAndTypeFound::WrongType;
	return false;
}

ULowEntryJsonObject* ULowEntryJsonObject::GetJsonObject(ULowEntryJsonObject* JsonObject, const FString& Key, ELowEntryJsonValueAndTypeFound& Branch)
{
	if(JsonObject == nullptr)
	{
		Branch = ELowEntryJsonValueAndTypeFound::NotFound;
		return nullptr;
	}
	TSharedPtr<FJsonValue>* Field = JsonObject->Data->Values.Find(Key);
	if((Field == nullptr) || !Field->IsValid())
	{
		Branch = ELowEntryJsonValueAndTypeFound::NotFound;
		return nullptr;
	}
	TSharedPtr<FJsonValue> Data = *Field;
	if(Data->Type == EJson::Object)
	{
		Branch = ELowEntryJsonValueAndTypeFound::Success;
		return ULowEntryJsonObject::Create(Data->AsObject());
	}
	Branch = ELowEntryJsonValueAndTypeFound::WrongType;
	return nullptr;
}

ULowEntryJsonArray* ULowEntryJsonObject::GetJsonArray(ULowEntryJsonObject* JsonObject, const FString& Key, ELowEntryJsonValueAndTypeFound& Branch)
{
	if(JsonObject == nullptr)
	{
		Branch = ELowEntryJsonValueAndTypeFound::NotFound;
		return nullptr;
	}
	TSharedPtr<FJsonValue>* Field = JsonObject->Data->Values.Find(Key);
	if((Field == nullptr) || !Field->IsValid())
	{
		Branch = ELowEntryJsonValueAndTypeFound::NotFound;
		return nullptr;
	}
	TSharedPtr<FJsonValue> Data = *Field;
	if(Data->Type == EJson::Array)
	{
		Branch = ELowEntryJsonValueAndTypeFound::Success;
		return ULowEntryJsonArray::Create(Data->AsArray());
	}
	Branch = ELowEntryJsonValueAndTypeFound::WrongType;
	return nullptr;
}

ULowEntryJsonValue* ULowEntryJsonObject::GetJsonValue(ULowEntryJsonObject* JsonObject, const FString& Key, ELowEntryJsonValueFound& Branch)
{
	if(JsonObject == nullptr)
	{
		Branch = ELowEntryJsonValueFound::NotFound;
		return nullptr;
	}
	TSharedPtr<FJsonValue>* Field = JsonObject->Data->Values.Find(Key);
	if((Field == nullptr) || !Field->IsValid())
	{
		Branch = ELowEntryJsonValueFound::NotFound;
		return nullptr;
	}
	Branch = ELowEntryJsonValueFound::Found;
	TSharedPtr<FJsonValue> Data = *Field;
	return ULowEntryJsonValue::Create(Data);
}


FString ULowEntryJsonObject::GetAsStringOrDefault(ULowEntryJsonObject* JsonObject, const FString& Key, const FString& Default)
{
	if(JsonObject == nullptr)
	{
		return Default;
	}
	TSharedPtr<FJsonValue>* Field = JsonObject->Data->Values.Find(Key);
	if((Field == nullptr) || !Field->IsValid())
	{
		return Default;
	}
	TSharedPtr<FJsonValue> Data = *Field;

	FString Value;
	if(Data->TryGetString(Value))
	{
		return Value;
	}
	return Default;
}

int32 ULowEntryJsonObject::GetAsIntegerOrDefault(ULowEntryJsonObject* JsonObject, const FString& Key, const int32 Default)
{
	if(JsonObject == nullptr)
	{
		return Default;
	}
	TSharedPtr<FJsonValue>* Field = JsonObject->Data->Values.Find(Key);
	if((Field == nullptr) || !Field->IsValid())
	{
		return Default;
	}
	TSharedPtr<FJsonValue> Data = *Field;

	int32 Value;
	if(Data->TryGetNumber(Value))
	{
		return Value;
	}
	return Default;
}

float ULowEntryJsonObject::GetAsFloatOrDefault(ULowEntryJsonObject* JsonObject, const FString& Key, const float Default)
{
	if(JsonObject == nullptr)
	{
		return Default;
	}
	TSharedPtr<FJsonValue>* Field = JsonObject->Data->Values.Find(Key);
	if((Field == nullptr) || !Field->IsValid())
	{
		return Default;
	}
	TSharedPtr<FJsonValue> Data = *Field;

	double Value;
	if(Data->TryGetNumber(Value))
	{
		return (float) Value;
	}
	return Default;
}

uint8 ULowEntryJsonObject::GetAsByteOrDefault(ULowEntryJsonObject* JsonObject, const FString& Key, const uint8 Default)
{
	if(JsonObject == nullptr)
	{
		return Default;
	}
	TSharedPtr<FJsonValue>* Field = JsonObject->Data->Values.Find(Key);
	if((Field == nullptr) || !Field->IsValid())
	{
		return Default;
	}
	TSharedPtr<FJsonValue> Data = *Field;

	int32 Value;
	if(Data->TryGetNumber(Value))
	{
		if((Value >= 0) && (Value <= 255))
		{
			return (uint8) Value;
		}
	}
	return Default;
}

bool ULowEntryJsonObject::GetAsBooleanOrDefault(ULowEntryJsonObject* JsonObject, const FString& Key, const bool Default)
{
	if(JsonObject == nullptr)
	{
		return Default;
	}
	TSharedPtr<FJsonValue>* Field = JsonObject->Data->Values.Find(Key);
	if((Field == nullptr) || !Field->IsValid())
	{
		return Default;
	}
	TSharedPtr<FJsonValue> Data = *Field;

	bool Value;
	if(Data->TryGetBool(Value))
	{
		return Value;
	}
	return Default;
}

ULowEntryJsonObject* ULowEntryJsonObject::GetAsJsonObjectOrDefault(ULowEntryJsonObject* JsonObject, const FString& Key, ULowEntryJsonObject* Default)
{
	//if(Default == nullptr)
	//{
	//	Default = ULowEntryJsonObject::Create();
	//}

	if(JsonObject == nullptr)
	{
		return Default;
	}
	TSharedPtr<FJsonValue>* Field = JsonObject->Data->Values.Find(Key);
	if((Field == nullptr) || !Field->IsValid())
	{
		return Default;
	}
	TSharedPtr<FJsonValue> Data = *Field;

	const TSharedPtr<FJsonObject>* Value;
	if(Data->TryGetObject(Value))
	{
		return ULowEntryJsonObject::Create(*Value);
	}
	return Default;
}

ULowEntryJsonArray* ULowEntryJsonObject::GetAsJsonArrayOrDefault(ULowEntryJsonObject* JsonObject, const FString& Key, ULowEntryJsonArray* Default)
{
	//if(Default == nullptr)
	//{
	//	Default = ULowEntryJsonArray::Create();
	//}

	if(JsonObject == nullptr)
	{
		return Default;
	}
	TSharedPtr<FJsonValue>* Field = JsonObject->Data->Values.Find(Key);
	if((Field == nullptr) || !Field->IsValid())
	{
		return Default;
	}
	TSharedPtr<FJsonValue> Data = *Field;

	const TArray<TSharedPtr<FJsonValue>>* Value;
	if(Data->TryGetArray(Value))
	{
		return ULowEntryJsonArray::Create(*Value);
	}
	return Default;
}

ULowEntryJsonValue* ULowEntryJsonObject::GetAsJsonValueOrDefault(ULowEntryJsonObject* JsonObject, const FString& Key, ULowEntryJsonValue* Default)
{
	//if(Default == nullptr)
	//{
	//	Default = ULowEntryJsonArray::Create();
	//}

	if(JsonObject == nullptr)
	{
		return Default;
	}
	TSharedPtr<FJsonValue>* Field = JsonObject->Data->Values.Find(Key);
	if((Field == nullptr) || !Field->IsValid())
	{
		return Default;
	}
	TSharedPtr<FJsonValue> Data = *Field;

	return ULowEntryJsonValue::Create(Data);
}


TArray<FString> ULowEntryJsonObject::GetKeys(ULowEntryJsonObject* JsonObject)
{
	TArray<FString> Keys;
	if(JsonObject != nullptr)
	{
		JsonObject->Data->Values.GenerateKeyArray(Keys);
	}
	return Keys;
}

ULowEntryJsonArray* ULowEntryJsonObject::GetValues(ULowEntryJsonObject* JsonObject)
{
	TArray<TSharedPtr<FJsonValue>> Values;
	if(JsonObject != nullptr)
	{
		JsonObject->Data->Values.GenerateValueArray(Values);
	}
	return ULowEntryJsonArray::Create(Values);
}

int32 ULowEntryJsonObject::Length(ULowEntryJsonObject* JsonObject)
{
	if(JsonObject != nullptr)
	{
		return JsonObject->Data->Values.Num();
	}
	return 0;
}


void ULowEntryJsonObject::Append(ULowEntryJsonObject* JsonObject, ULowEntryJsonObject* AppendJsonObject, const bool OverrideDuplicates)
{
	if(JsonObject == nullptr)
	{
		return;
	}
	if(AppendJsonObject == nullptr)
	{
		return;
	}

	if(OverrideDuplicates)
	{
		JsonObject->Data->Values.Append(AppendJsonObject->Data->Values);
	}
	else
	{
		for(auto i = AppendJsonObject->Data->Values.CreateConstIterator(); i; ++i)
		{
			const TSharedPtr<FJsonValue>* Field = JsonObject->Data->Values.Find(i->Key);
			if(!Field || !Field->IsValid())
			{
				JsonObject->Data->Values.Add(i->Key, i->Value);
			}
		}
	}
}


void ULowEntryJsonObject::Sort(ULowEntryJsonObject* JsonObject, const bool Reversed)
{
	if(JsonObject == nullptr)
	{
		return;
	}

	if(!Reversed)
	{
		JsonObject->Data->Values.KeySort([](const FString& One, const FString& Two)
		{
			return One < Two;
		});
	}
	else
	{
		JsonObject->Data->Values.KeySort([] (const FString& One, const FString& Two){
			return One > Two;
		});
	}
}
