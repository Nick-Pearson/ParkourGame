#include "LowEntryJsonDebugLibrary.h"
#include "LowEntryJsonObject.h"
#include "LowEntryJsonArray.h"
#include "LowEntryJsonValue.h"


#define LOW_ENTRY_JSON_DEBUG_START() ULowEntryJsonObject* Container = ULowEntryJsonObject::Create();
#define LOW_ENTRY_JSON_DEBUG_END() if(Label.Len() > 0){ ULowEntryJsonDebugLibrary::KismetSystemLibraryPrintString(WorldContextObject, Label + ": " + ULowEntryJsonDebugLibrary::TrimJsonString(ULowEntryJsonObject::ToJsonString(Container, true)), ScreenDurationTime, bPrintToScreen, bPrintToLog, TextColor); }else{ ULowEntryJsonDebugLibrary::KismetSystemLibraryPrintString(WorldContextObject, ULowEntryJsonDebugLibrary::TrimJsonString(ULowEntryJsonObject::ToJsonString(Container, true)), ScreenDurationTime, bPrintToScreen, bPrintToLog, TextColor); }
#define LOW_ENTRY_JSON_DEBUG_FUNCTION_PARAMETERS() const FString& Label, const float ScreenDurationTime, const bool bPrintToScreen, const bool bPrintToLog, const FLinearColor TextColor


// init >>
	ULowEntryJsonDebugLibrary::ULowEntryJsonDebugLibrary(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
	{
	}
// init <<


void ULowEntryJsonDebugLibrary::KismetSystemLibraryPrintString(UObject* WorldContextObject, const FString& InString, const float ScreenDurationTime, const bool bPrintToScreen, const bool bPrintToLog, const FLinearColor TextColor)
{
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	WorldContextObject = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::ReturnNull);
	FString Prefix;
	if(WorldContextObject)
	{
		UWorld *World = WorldContextObject->GetWorld();
		if(World->WorldType == EWorldType::PIE)
		{
			switch(World->GetNetMode())
			{
			case NM_Client:
				Prefix = FString::Printf(TEXT("Client %d: "), GPlayInEditorID - 1);
				break;
			case NM_DedicatedServer:
			case NM_ListenServer:
				Prefix = FString::Printf(TEXT("Server: "));
				break;
			}
		}
	}

	const FString FinalString = Prefix + InString;

	if(bPrintToLog)
	{
		UE_LOG(LogBlueprintUserMessages, Log, TEXT("%s"), *FinalString);
	}
	else
	{
		UE_LOG(LogBlueprintUserMessages, Verbose, TEXT("%s"), *FinalString);
	}

	if(bPrintToScreen)
	{
		if(GAreScreenMessagesEnabled)
		{
			GEngine->AddOnScreenDebugMessage((uint64) -1, ScreenDurationTime, TextColor.ToFColor(true), FinalString);
		}
	}
#endif
}

FString ULowEntryJsonDebugLibrary::TrimJsonString(const FString& String)
{
	FString LocalString = String;
	LocalString.TrimStartAndEndInline();
	LocalString.RemoveFromStart(TEXT("{"));
	LocalString.RemoveFromEnd(TEXT("}"));
	LocalString.TrimStartAndEndInline();
	LocalString.RemoveFromStart(TEXT("\"\": "));
	return LocalString;
}


void ULowEntryJsonDebugLibrary::PrintJsonObject(UObject* WorldContextObject, ULowEntryJsonObject* JsonObject, LOW_ENTRY_JSON_DEBUG_FUNCTION_PARAMETERS())
{
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	LOW_ENTRY_JSON_DEBUG_START()
		ULowEntryJsonObject::SetJsonObject(Container, TEXT(""), JsonObject);
	LOW_ENTRY_JSON_DEBUG_END()
#endif
}

void ULowEntryJsonDebugLibrary::PrintJsonArray(UObject* WorldContextObject, ULowEntryJsonArray* JsonArray, LOW_ENTRY_JSON_DEBUG_FUNCTION_PARAMETERS())
{
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	LOW_ENTRY_JSON_DEBUG_START()
		ULowEntryJsonObject::SetJsonArray(Container, TEXT(""), JsonArray);
	LOW_ENTRY_JSON_DEBUG_END()
#endif
}

void ULowEntryJsonDebugLibrary::PrintJsonValue(UObject* WorldContextObject, ULowEntryJsonValue* JsonValue, LOW_ENTRY_JSON_DEBUG_FUNCTION_PARAMETERS())
{
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	LOW_ENTRY_JSON_DEBUG_START()
		ULowEntryJsonObject::SetJsonValue(Container, TEXT(""), JsonValue);
	LOW_ENTRY_JSON_DEBUG_END()
#endif
}

void ULowEntryJsonDebugLibrary::PrintStringEscaped(UObject* WorldContextObject, const FString& String, LOW_ENTRY_JSON_DEBUG_FUNCTION_PARAMETERS())
{
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	LOW_ENTRY_JSON_DEBUG_START()
		ULowEntryJsonObject::SetString(Container, TEXT(""), String);
	LOW_ENTRY_JSON_DEBUG_END()
#endif
}

void ULowEntryJsonDebugLibrary::PrintInteger(UObject* WorldContextObject, const int32 Integer, LOW_ENTRY_JSON_DEBUG_FUNCTION_PARAMETERS())
{
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	LOW_ENTRY_JSON_DEBUG_START()
		ULowEntryJsonObject::SetInteger(Container, TEXT(""), Integer);
	LOW_ENTRY_JSON_DEBUG_END()
#endif
}

void ULowEntryJsonDebugLibrary::PrintFloat(UObject* WorldContextObject, const float Float, LOW_ENTRY_JSON_DEBUG_FUNCTION_PARAMETERS())
{
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	LOW_ENTRY_JSON_DEBUG_START()
		ULowEntryJsonObject::SetFloat(Container, TEXT(""), Float);
	LOW_ENTRY_JSON_DEBUG_END()
#endif
}

void ULowEntryJsonDebugLibrary::PrintByte(UObject* WorldContextObject, const uint8 Byte, LOW_ENTRY_JSON_DEBUG_FUNCTION_PARAMETERS())
{
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	LOW_ENTRY_JSON_DEBUG_START()
		ULowEntryJsonObject::SetByte(Container, TEXT(""), Byte);
	LOW_ENTRY_JSON_DEBUG_END()
#endif
}

void ULowEntryJsonDebugLibrary::PrintBoolean(UObject* WorldContextObject, const bool Boolean, LOW_ENTRY_JSON_DEBUG_FUNCTION_PARAMETERS())
{
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	LOW_ENTRY_JSON_DEBUG_START()
		ULowEntryJsonObject::SetBoolean(Container, TEXT(""), Boolean);
	LOW_ENTRY_JSON_DEBUG_END()
#endif
}

void ULowEntryJsonDebugLibrary::PrintJsonObjectArray(UObject* WorldContextObject, const TArray<ULowEntryJsonObject*>& JsonObjectArray, LOW_ENTRY_JSON_DEBUG_FUNCTION_PARAMETERS())
{
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	LOW_ENTRY_JSON_DEBUG_START()
		ULowEntryJsonObject::SetJsonArray(Container, TEXT(""), ULowEntryJsonArray::CreateFromJsonObjectArray(JsonObjectArray));
	LOW_ENTRY_JSON_DEBUG_END()
#endif
}

void ULowEntryJsonDebugLibrary::PrintJsonArrayArray(UObject* WorldContextObject, const TArray<ULowEntryJsonArray*>& JsonArrayArray, LOW_ENTRY_JSON_DEBUG_FUNCTION_PARAMETERS())
{
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	LOW_ENTRY_JSON_DEBUG_START()
		ULowEntryJsonObject::SetJsonArray(Container, TEXT(""), ULowEntryJsonArray::CreateFromJsonArrayArray(JsonArrayArray));
	LOW_ENTRY_JSON_DEBUG_END()
#endif
}

void ULowEntryJsonDebugLibrary::PrintJsonValueArray(UObject* WorldContextObject, const TArray<ULowEntryJsonValue*>& JsonValueArray, LOW_ENTRY_JSON_DEBUG_FUNCTION_PARAMETERS())
{
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	LOW_ENTRY_JSON_DEBUG_START()
		ULowEntryJsonObject::SetJsonArray(Container, TEXT(""), ULowEntryJsonArray::CreateFromJsonValueArray(JsonValueArray));
	LOW_ENTRY_JSON_DEBUG_END()
#endif
}

void ULowEntryJsonDebugLibrary::PrintStringArrayEscaped(UObject* WorldContextObject, const TArray<FString>& StringArray, LOW_ENTRY_JSON_DEBUG_FUNCTION_PARAMETERS())
{
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	LOW_ENTRY_JSON_DEBUG_START()
		ULowEntryJsonObject::SetJsonArray(Container, TEXT(""), ULowEntryJsonArray::CreateFromStringArray(StringArray));
	LOW_ENTRY_JSON_DEBUG_END()
#endif
}

void ULowEntryJsonDebugLibrary::PrintIntegerArray(UObject* WorldContextObject, const TArray<int32>& IntegerArray, LOW_ENTRY_JSON_DEBUG_FUNCTION_PARAMETERS())
{
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	LOW_ENTRY_JSON_DEBUG_START()
		ULowEntryJsonObject::SetJsonArray(Container, TEXT(""), ULowEntryJsonArray::CreateFromIntegerArray(IntegerArray));
	LOW_ENTRY_JSON_DEBUG_END()
#endif
}

void ULowEntryJsonDebugLibrary::PrintFloatArray(UObject* WorldContextObject, const TArray<float>& FloatArray, LOW_ENTRY_JSON_DEBUG_FUNCTION_PARAMETERS())
{
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	LOW_ENTRY_JSON_DEBUG_START()
		ULowEntryJsonObject::SetJsonArray(Container, TEXT(""), ULowEntryJsonArray::CreateFromFloatArray(FloatArray));
	LOW_ENTRY_JSON_DEBUG_END()
#endif
}

void ULowEntryJsonDebugLibrary::PrintByteArray(UObject* WorldContextObject, const TArray<uint8>& ByteArray, LOW_ENTRY_JSON_DEBUG_FUNCTION_PARAMETERS())
{
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	LOW_ENTRY_JSON_DEBUG_START()
		ULowEntryJsonObject::SetJsonArray(Container, TEXT(""), ULowEntryJsonArray::CreateFromByteArray(ByteArray));
	LOW_ENTRY_JSON_DEBUG_END()
#endif
}

void ULowEntryJsonDebugLibrary::PrintBooleanArray(UObject* WorldContextObject, const TArray<bool>& BooleanArray, LOW_ENTRY_JSON_DEBUG_FUNCTION_PARAMETERS())
{
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	LOW_ENTRY_JSON_DEBUG_START()
		ULowEntryJsonObject::SetJsonArray(Container, TEXT(""), ULowEntryJsonArray::CreateFromBooleanArray(BooleanArray));
	LOW_ENTRY_JSON_DEBUG_END()
#endif
}
