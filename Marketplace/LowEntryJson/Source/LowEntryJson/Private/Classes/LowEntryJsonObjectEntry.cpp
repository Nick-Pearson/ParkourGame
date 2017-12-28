#include "LowEntryJsonObjectEntry.h"
#include "LowEntryJsonObject.h"
#include "LowEntryJsonArray.h"
#include "LowEntryJsonValue.h"


// init >>
	ULowEntryJsonObjectEntry::ULowEntryJsonObjectEntry(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
	{
		Value = ULowEntryJsonValue::Create();
	}

	ULowEntryJsonObjectEntry* ULowEntryJsonObjectEntry::Create(const FString& Key, ULowEntryJsonValue* Value)
	{
		ULowEntryJsonObjectEntry* Instance = NewObject<ULowEntryJsonObjectEntry>();
		Instance->Key = Key;
		if(Value != nullptr)
		{
			Instance->Value = Value;
		}
		return Instance;
	}

	ULowEntryJsonObjectEntry* ULowEntryJsonObjectEntry::CreateFromNull(const FString& Key)
	{
		ULowEntryJsonObjectEntry* Instance = NewObject<ULowEntryJsonObjectEntry>();
		Instance->Key = Key;
		Instance->Value = ULowEntryJsonValue::CreateFromNull();
		return Instance;
	}

	ULowEntryJsonObjectEntry* ULowEntryJsonObjectEntry::CreateFromString(const FString& Key, const FString& Value)
	{
		ULowEntryJsonObjectEntry* Instance = NewObject<ULowEntryJsonObjectEntry>();
		Instance->Key = Key;
		Instance->Value = ULowEntryJsonValue::CreateFromString(Value);
		return Instance;
	}

	ULowEntryJsonObjectEntry* ULowEntryJsonObjectEntry::CreateFromInteger(const FString& Key, const int32 Value)
	{
		ULowEntryJsonObjectEntry* Instance = NewObject<ULowEntryJsonObjectEntry>();
		Instance->Key = Key;
		Instance->Value = ULowEntryJsonValue::CreateFromInteger(Value);
		return Instance;
	}

	ULowEntryJsonObjectEntry* ULowEntryJsonObjectEntry::CreateFromFloat(const FString& Key, const float Value)
	{
		ULowEntryJsonObjectEntry* Instance = NewObject<ULowEntryJsonObjectEntry>();
		Instance->Key = Key;
		Instance->Value = ULowEntryJsonValue::CreateFromFloat(Value);
		return Instance;
	}

	ULowEntryJsonObjectEntry* ULowEntryJsonObjectEntry::CreateFromByte(const FString& Key, const uint8 Value)
	{
		ULowEntryJsonObjectEntry* Instance = NewObject<ULowEntryJsonObjectEntry>();
		Instance->Key = Key;
		Instance->Value = ULowEntryJsonValue::CreateFromByte(Value);
		return Instance;
	}

	ULowEntryJsonObjectEntry* ULowEntryJsonObjectEntry::CreateFromBoolean(const FString& Key, const bool Value)
	{
		ULowEntryJsonObjectEntry* Instance = NewObject<ULowEntryJsonObjectEntry>();
		Instance->Key = Key;
		Instance->Value = ULowEntryJsonValue::CreateFromBoolean(Value);
		return Instance;
	}

	ULowEntryJsonObjectEntry* ULowEntryJsonObjectEntry::CreateFromJsonObject(const FString& Key, ULowEntryJsonObject* Value)
	{
		ULowEntryJsonObjectEntry* Instance = NewObject<ULowEntryJsonObjectEntry>();
		Instance->Key = Key;
		Instance->Value = ULowEntryJsonValue::CreateFromJsonObject(Value);
		return Instance;
	}

	ULowEntryJsonObjectEntry* ULowEntryJsonObjectEntry::CreateFromJsonArray(const FString& Key, ULowEntryJsonArray* Value)
	{
		ULowEntryJsonObjectEntry* Instance = NewObject<ULowEntryJsonObjectEntry>();
		Instance->Key = Key;
		Instance->Value = ULowEntryJsonValue::CreateFromJsonArray(Value);
		return Instance;
	}
// init <<
