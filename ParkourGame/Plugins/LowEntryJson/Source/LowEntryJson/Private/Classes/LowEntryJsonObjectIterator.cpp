#include "LowEntryJsonObjectIterator.h"
#include "LowEntryJsonObject.h"
#include "LowEntryJsonArray.h"
#include "LowEntryJsonValue.h"


// init >>
	ULowEntryJsonObjectIterator::ULowEntryJsonObjectIterator(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
	{
	}

	ULowEntryJsonObjectIterator* ULowEntryJsonObjectIterator::Create(ULowEntryJsonObject* JsonObject)
	{
		ULowEntryJsonObjectIterator* Instance = NewObject<ULowEntryJsonObjectIterator>();
		if(JsonObject != nullptr)
		{
			TMap<FString, TSharedPtr<FJsonValue>>::TIterator Iterator = JsonObject->Data->Values.CreateIterator();
			while(Iterator)
			{
				Instance->Keys.Add(Iterator.Key());
				Instance->Values.Add(Iterator.Value());
				++Iterator;
			}
		}
		return Instance;
	}

	ULowEntryJsonObjectIterator* ULowEntryJsonObjectIterator::Clone(ULowEntryJsonObjectIterator* JsonObjectIterator)
	{
		ULowEntryJsonObjectIterator* Instance = NewObject<ULowEntryJsonObjectIterator>();
		if(JsonObjectIterator != nullptr)
		{
			Instance->Keys = JsonObjectIterator->Keys;
			Instance->Values = JsonObjectIterator->Values;
		}
		return Instance;
	}
// init <<


void ULowEntryJsonObjectIterator::Get(ULowEntryJsonObjectIterator* JsonObjectIterator, const int32 Index, FString& Key, ULowEntryJsonValue*& JsonValue, ELowEntryJsonValueFound& Branch)
{
	if(JsonObjectIterator == nullptr)
	{
		Branch = ELowEntryJsonValueFound::NotFound;
	}
	else if(!JsonObjectIterator->Keys.IsValidIndex(Index) || !JsonObjectIterator->Values.IsValidIndex(Index))
	{
		Branch = ELowEntryJsonValueFound::NotFound;
	}
	else
	{
		Branch = ELowEntryJsonValueFound::Found;
		Key = JsonObjectIterator->Keys[Index];
		JsonValue = ULowEntryJsonValue::Create(JsonObjectIterator->Values[Index]);
	}
}


TArray<FString> ULowEntryJsonObjectIterator::GetKeys(ULowEntryJsonObjectIterator* JsonObjectIterator)
{
	if(JsonObjectIterator == nullptr)
	{
		return TArray<FString>();
	}
	return JsonObjectIterator->Keys;
}

ULowEntryJsonArray* ULowEntryJsonObjectIterator::GetValues(ULowEntryJsonObjectIterator* JsonObjectIterator)
{
	TArray<TSharedPtr<FJsonValue>> Values;
	if(JsonObjectIterator != nullptr)
	{
		Values = JsonObjectIterator->Values;
	}
	return ULowEntryJsonArray::Create(Values);
}

int32 ULowEntryJsonObjectIterator::Length(ULowEntryJsonObjectIterator* JsonObjectIterator)
{
	if(JsonObjectIterator != nullptr)
	{
		return JsonObjectIterator->Keys.Num();
	}
	return 0;
}
