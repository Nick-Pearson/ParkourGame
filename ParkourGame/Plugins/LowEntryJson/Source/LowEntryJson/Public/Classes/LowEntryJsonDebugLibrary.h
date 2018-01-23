#pragma once


#include "CoreMinimal.h"

#include "Engine/World.h"
#include "Engine/Engine.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "Kismet/KismetSystemLibrary.h"

#include "LowEntryJsonDebugLibrary.generated.h"


class ULowEntryJsonObject;
class ULowEntryJsonArray;
class ULowEntryJsonValue;


UCLASS()
class LOWENTRYJSON_API ULowEntryJsonDebugLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()


public:
	static void KismetSystemLibraryPrintString(UObject* WorldContextObject, const FString& InString, const float ScreenDurationTime, const bool bPrintToScreen, const bool bPrintToLog, const FLinearColor TextColor);
	static FString TrimJsonString(const FString& String);


public:
	/**
	* Creates a Json Object, sets the given variable as a value, converts the created Json Object to a Json String, formats the Json String, and prints the formatted Json String using KismetSystemLibrary's PrintString.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Debug|Print", Meta = (WorldContext = "WorldContextObject", CallableWithoutWorldContext, Keywords = "log print", AdvancedDisplay = "3"))
		static void PrintJsonObject(UObject* WorldContextObject, ULowEntryJsonObject* JsonObject, const FString& Label, const float ScreenDurationTime = 5.0f, const bool bPrintToScreen = true, const bool bPrintToLog = true, const FLinearColor TextColor = FLinearColor(0.0, 0.66, 1.0));

	/**
	* Creates a Json Object, sets the given variable as a value, converts the created Json Object to a Json String, formats the Json String, and prints the formatted Json String using KismetSystemLibrary's PrintString.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Debug|Print", Meta = (WorldContext = "WorldContextObject", CallableWithoutWorldContext, Keywords = "log print", AdvancedDisplay = "3"))
		static void PrintJsonArray(UObject* WorldContextObject, ULowEntryJsonArray* JsonArray, const FString& Label, const float ScreenDurationTime = 5.0f, const bool bPrintToScreen = true, const bool bPrintToLog = true, const FLinearColor TextColor = FLinearColor(0.0, 0.66, 1.0));

	/**
	* Creates a Json Object, sets the given variable as a value, converts the created Json Object to a Json String, formats the Json String, and prints the formatted Json String using KismetSystemLibrary's PrintString.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Debug|Print", Meta = (WorldContext = "WorldContextObject", CallableWithoutWorldContext, Keywords = "log print", AdvancedDisplay = "3"))
		static void PrintJsonValue(UObject* WorldContextObject, ULowEntryJsonValue* JsonValue, const FString& Label, const float ScreenDurationTime = 5.0f, const bool bPrintToScreen = true, const bool bPrintToLog = true, const FLinearColor TextColor = FLinearColor(0.0, 0.66, 1.0));

	/**
	* Creates a Json Object, sets the given variable as a value, converts the created Json Object to a Json String, formats the Json String, and prints the formatted Json String using KismetSystemLibrary's PrintString.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Debug|Print", Meta = (WorldContext = "WorldContextObject", CallableWithoutWorldContext, Keywords = "log print", AdvancedDisplay = "3", DisplayName = "Print String (Escaped)"))
		static void PrintStringEscaped(UObject* WorldContextObject, const FString& String, const FString& Label, const float ScreenDurationTime = 5.0f, const bool bPrintToScreen = true, const bool bPrintToLog = true, const FLinearColor TextColor = FLinearColor(0.0, 0.66, 1.0));

	/**
	* Creates a Json Object, sets the given variable as a value, converts the created Json Object to a Json String, formats the Json String, and prints the formatted Json String using KismetSystemLibrary's PrintString.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Debug|Print", Meta = (WorldContext = "WorldContextObject", CallableWithoutWorldContext, Keywords = "log print", AdvancedDisplay = "3"))
		static void PrintInteger(UObject* WorldContextObject, const int32 Integer, const FString& Label, const float ScreenDurationTime = 5.0f, const bool bPrintToScreen = true, const bool bPrintToLog = true, const FLinearColor TextColor = FLinearColor(0.0, 0.66, 1.0));

	/**
	* Creates a Json Object, sets the given variable as a value, converts the created Json Object to a Json String, formats the Json String, and prints the formatted Json String using KismetSystemLibrary's PrintString.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Debug|Print", Meta = (WorldContext = "WorldContextObject", CallableWithoutWorldContext, Keywords = "log print", AdvancedDisplay = "3"))
		static void PrintFloat(UObject* WorldContextObject, const float Float, const FString& Label, const float ScreenDurationTime = 5.0f, const bool bPrintToScreen = true, const bool bPrintToLog = true, const FLinearColor TextColor = FLinearColor(0.0, 0.66, 1.0));

	/**
	* Creates a Json Object, sets the given variable as a value, converts the created Json Object to a Json String, formats the Json String, and prints the formatted Json String using KismetSystemLibrary's PrintString.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Debug|Print", Meta = (WorldContext = "WorldContextObject", CallableWithoutWorldContext, Keywords = "log print", AdvancedDisplay = "3"))
		static void PrintByte(UObject* WorldContextObject, const uint8 Byte, const FString& Label, const float ScreenDurationTime = 5.0f, const bool bPrintToScreen = true, const bool bPrintToLog = true, const FLinearColor TextColor = FLinearColor(0.0, 0.66, 1.0));

	/**
	* Creates a Json Object, sets the given variable as a value, converts the created Json Object to a Json String, formats the Json String, and prints the formatted Json String using KismetSystemLibrary's PrintString.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Debug|Print", Meta = (WorldContext = "WorldContextObject", CallableWithoutWorldContext, Keywords = "log print", AdvancedDisplay = "3"))
		static void PrintBoolean(UObject* WorldContextObject, const bool Boolean, const FString& Label, const float ScreenDurationTime = 5.0f, const bool bPrintToScreen = true, const bool bPrintToLog = true, const FLinearColor TextColor = FLinearColor(0.0, 0.66, 1.0));

	/**
	* Creates a Json Object, sets the given variable as a value, converts the created Json Object to a Json String, formats the Json String, and prints the formatted Json String using KismetSystemLibrary's PrintString.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Debug|Print", Meta = (WorldContext = "WorldContextObject", CallableWithoutWorldContext, Keywords = "log print", AdvancedDisplay = "3"))
		static void PrintJsonObjectArray(UObject* WorldContextObject, const TArray<ULowEntryJsonObject*>& JsonObjectArray, const FString& Label, const float ScreenDurationTime = 5.0f, const bool bPrintToScreen = true, const bool bPrintToLog = true, const FLinearColor TextColor = FLinearColor(0.0, 0.66, 1.0));

	/**
	* Creates a Json Object, sets the given variable as a value, converts the created Json Object to a Json String, formats the Json String, and prints the formatted Json String using KismetSystemLibrary's PrintString.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Debug|Print", Meta = (WorldContext = "WorldContextObject", CallableWithoutWorldContext, Keywords = "log print", AdvancedDisplay = "3"))
		static void PrintJsonArrayArray(UObject* WorldContextObject, const TArray<ULowEntryJsonArray*>& JsonArrayArray, const FString& Label, const float ScreenDurationTime = 5.0f, const bool bPrintToScreen = true, const bool bPrintToLog = true, const FLinearColor TextColor = FLinearColor(0.0, 0.66, 1.0));

	/**
	* Creates a Json Object, sets the given variable as a value, converts the created Json Object to a Json String, formats the Json String, and prints the formatted Json String using KismetSystemLibrary's PrintString.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Debug|Print", Meta = (WorldContext = "WorldContextObject", CallableWithoutWorldContext, Keywords = "log print", AdvancedDisplay = "3"))
		static void PrintJsonValueArray(UObject* WorldContextObject, const TArray<ULowEntryJsonValue*>& JsonValueArray, const FString& Label, const float ScreenDurationTime = 5.0f, const bool bPrintToScreen = true, const bool bPrintToLog = true, const FLinearColor TextColor = FLinearColor(0.0, 0.66, 1.0));

	/**
	* Creates a Json Object, sets the given variable as a value, converts the created Json Object to a Json String, formats the Json String, and prints the formatted Json String using KismetSystemLibrary's PrintString.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Debug|Print", Meta = (WorldContext = "WorldContextObject", CallableWithoutWorldContext, Keywords = "log print", AdvancedDisplay = "3", DisplayName = "Print String Array (Escaped)"))
		static void PrintStringArrayEscaped(UObject* WorldContextObject, const TArray<FString>& StringArray, const FString& Label, const float ScreenDurationTime = 5.0f, const bool bPrintToScreen = true, const bool bPrintToLog = true, const FLinearColor TextColor = FLinearColor(0.0, 0.66, 1.0));

	/**
	* Creates a Json Object, sets the given variable as a value, converts the created Json Object to a Json String, formats the Json String, and prints the formatted Json String using KismetSystemLibrary's PrintString.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Debug|Print", Meta = (WorldContext = "WorldContextObject", CallableWithoutWorldContext, Keywords = "log print", AdvancedDisplay = "3"))
		static void PrintIntegerArray(UObject* WorldContextObject, const TArray<int32>& IntegerArray, const FString& Label, const float ScreenDurationTime = 5.0f, const bool bPrintToScreen = true, const bool bPrintToLog = true, const FLinearColor TextColor = FLinearColor(0.0, 0.66, 1.0));

	/**
	* Creates a Json Object, sets the given variable as a value, converts the created Json Object to a Json String, formats the Json String, and prints the formatted Json String using KismetSystemLibrary's PrintString.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Debug|Print", Meta = (WorldContext = "WorldContextObject", CallableWithoutWorldContext, Keywords = "log print", AdvancedDisplay = "3"))
		static void PrintFloatArray(UObject* WorldContextObject, const TArray<float>& FloatArray, const FString& Label, const float ScreenDurationTime = 5.0f, const bool bPrintToScreen = true, const bool bPrintToLog = true, const FLinearColor TextColor = FLinearColor(0.0, 0.66, 1.0));

	/**
	* Creates a Json Object, sets the given variable as a value, converts the created Json Object to a Json String, formats the Json String, and prints the formatted Json String using KismetSystemLibrary's PrintString.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Debug|Print", Meta = (WorldContext = "WorldContextObject", CallableWithoutWorldContext, Keywords = "log print", AdvancedDisplay = "3"))
		static void PrintByteArray(UObject* WorldContextObject, const TArray<uint8>& ByteArray, const FString& Label, const float ScreenDurationTime = 5.0f, const bool bPrintToScreen = true, const bool bPrintToLog = true, const FLinearColor TextColor = FLinearColor(0.0, 0.66, 1.0));

	/**
	* Creates a Json Object, sets the given variable as a value, converts the created Json Object to a Json String, formats the Json String, and prints the formatted Json String using KismetSystemLibrary's PrintString.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Debug|Print", Meta = (WorldContext = "WorldContextObject", CallableWithoutWorldContext, Keywords = "log print", AdvancedDisplay = "3"))
		static void PrintBooleanArray(UObject* WorldContextObject, const TArray<bool>& BooleanArray, const FString& Label, const float ScreenDurationTime = 5.0f, const bool bPrintToScreen = true, const bool bPrintToLog = true, const FLinearColor TextColor = FLinearColor(0.0, 0.66, 1.0));
};
