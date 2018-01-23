#pragma once


#include "CoreMinimal.h"

#include "Kismet/BlueprintFunctionLibrary.h"

#include "Json.h"

#include "ELowEntryJsonType.h"
#include "ELowEntryJsonTypeFound.h"
#include "ELowEntryJsonValueFound.h"
#include "ELowEntryJsonValueAndTypeFound.h"
#include "ELowEntryJsonParseResult.h"

#include "LowEntryJsonLibrary.generated.h"


class ULowEntryJsonLibrary;
class ULowEntryJsonObject;
class ULowEntryJsonArray;
class ULowEntryJsonValue;
class ULowEntryJsonObjectIterator;
class ULowEntryJsonObjectEntry;


UCLASS()
class LOWENTRYJSON_API ULowEntryJsonLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()



public: // Json
	/**
	* Parses a Json String into a Json Object or a Json Array.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Json String|Parse", Meta = (DisplayName = "Parse Json String", ExpandEnumAsExecs = "Branch"))
		static void Json_ParseJsonString(const FString& JsonString, ULowEntryJsonObject*& ReturnJsonObjectValue, ULowEntryJsonArray*& ReturnJsonArrayValue, ELowEntryJsonParseResult& Branch);



public: // Object
	/**
	* Creates a new Json Object.
	*/
	/*UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Object|Create (Callable)", Meta = (DisplayName = "Create Json Object"))*/
		static ULowEntryJsonObject* Object_Create();

	/**
	* Creates a new Json Object.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Object|Create (Callable)", Meta = (DisplayName = "Create Json Object (Json Object Entry Array)"))
		static ULowEntryJsonObject* Object_CreateFromObjectEntryArray(const TArray<ULowEntryJsonObjectEntry*>& Array);

	/**
	* Creates a new Json Object and fills it with a copy of the data from the given Json Object.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Object|Create (Callable)", Meta = (DisplayName = "Clone"))
		static ULowEntryJsonObject* Object_Clone(ULowEntryJsonObject* JsonObject);

	/**
	* Creates a new Json Object and fills it with a copy of the data from the two given Json Objects.
	* When both of the two given Json Objects have a value set on same key, the key in the new Json Object will be set to the value that has been set in Json Object 2.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Object|Create (Callable)", Meta = (DisplayName = "Merge"))
		static ULowEntryJsonObject* Object_Merge(ULowEntryJsonObject* JsonObject1, ULowEntryJsonObject* JsonObject2);


	/**
	* Creates a new Json Object.
	*/
	/*UFUNCTION(BlueprintPure, Category = "Low Entry|Json|Object|Create (Pure)", Meta = (DisplayName = "Create Json Object"))*/
		static ULowEntryJsonObject* Object_Pure_Create();

	/**
	* Creates a new Json Object.
	*/
	UFUNCTION(BlueprintPure, Category = "Low Entry|Json|Object|Create (Pure)", Meta = (DisplayName = "Create Json Object (Json Object Entry Array)"))
		static ULowEntryJsonObject* Object_Pure_CreateFromObjectEntryArray(const TArray<ULowEntryJsonObjectEntry*>& Array);

	/**
	* Creates a new Json Object and fills it with a copy of the data from the given Json Object.
	*/
	UFUNCTION(BlueprintPure, Category = "Low Entry|Json|Object|Create (Pure)", Meta = (DisplayName = "Clone"))
		static ULowEntryJsonObject* Object_Pure_Clone(ULowEntryJsonObject* JsonObject);

	/**
	* Creates a new Json Object and fills it with a copy of the data from the two given Json Objects.
	* When both of the two given Json Objects have a value set on same key, the key in the new Json Object will be set to the value that has been set in Json Object 2.
	*/
	UFUNCTION(BlueprintPure, Category = "Low Entry|Json|Object|Create (Pure)", Meta = (DisplayName = "Merge"))
		static ULowEntryJsonObject* Object_Pure_Merge(ULowEntryJsonObject* JsonObject1, ULowEntryJsonObject* JsonObject2);


	/**
	* Creates a Json String from the given Json Object.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Json String|Stringify", Meta = (DisplayName = "To Json String (Json Object)"))
		static FString Object_ToJsonString(ULowEntryJsonObject* JsonObject, const bool PrettyPrint = true);

	/**
	* Creates a Json String from the given Json Object.
	*/
	UFUNCTION(BlueprintPure, Category = "Low Entry|Json|Json String|Stringify", Meta = (DisplayName = "To Json String (Json Object) (Pure)"))
		static FString Object_Pure_ToJsonString(ULowEntryJsonObject* JsonObject, const bool PrettyPrint = true);


	/**
	* Removes all entries from this Json Object, making it an empty Json Object.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Object|Other", Meta = (DisplayName = "Clear"))
		static void Object_Clear(ULowEntryJsonObject* JsonObject);

	/**
	* If there is an entry with the given key, it will be removed from this Json Object.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Object|Field", Meta = (DisplayName = "Unset"))
		static void Object_Unset(ULowEntryJsonObject* JsonObject, const FString& Key);

	/**
	* Returns true if this Json Object has an entry with the given key.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Object|Field", Meta = (DisplayName = "Is Set"))
		static bool Object_IsSet(ULowEntryJsonObject* JsonObject, const FString& Key);


	/**
	* Sets the value of the entry of the given key to null.
	* A new entry will be created if no entry with the given key yet exists.
	*
	* Note: This doesn't remove an entry because null is a valid value in Json, to remove an entry from a Json Object, use Unset.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Object|Field", Meta = (DisplayName = "Set Null"))
		static void Object_SetNull(ULowEntryJsonObject* JsonObject, const FString& Key);

	/**
	* Sets the value of the entry of the given key to the given String.
	* A new entry will be created if no entry with the given key yet exists.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Object|Field", Meta = (DisplayName = "Set String"))
		static void Object_SetString(ULowEntryJsonObject* JsonObject, const FString& Key, const FString& Value);

	/**
	* Sets the value of the entry of the given key to the given Integer.
	* A new entry will be created if no entry with the given key yet exists.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Object|Field", Meta = (DisplayName = "Set Integer"))
		static void Object_SetInteger(ULowEntryJsonObject* JsonObject, const FString& Key, const int32 Value);

	/**
	* Sets the value of the entry of the given key to the given Float.
	* A new entry will be created if no entry with the given key yet exists.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Object|Field", Meta = (DisplayName = "Set Float"))
		static void Object_SetFloat(ULowEntryJsonObject* JsonObject, const FString& Key, const float Value);

	/**
	* Sets the value of the entry of the given key to the given Byte.
	* A new entry will be created if no entry with the given key yet exists.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Object|Field", Meta = (DisplayName = "Set Byte"))
		static void Object_SetByte(ULowEntryJsonObject* JsonObject, const FString& Key, const uint8 Value);

	/**
	* Sets the value of the entry of the given key to the given Boolean.
	* A new entry will be created if no entry with the given key yet exists.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Object|Field", Meta = (DisplayName = "Set Boolean"))
		static void Object_SetBoolean(ULowEntryJsonObject* JsonObject, const FString& Key, const bool Value);

	/**
	* Sets the value of the entry of the given key to a copy of the given Json Object.
	* A new entry will be created if no entry with the given key yet exists.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Object|Field", Meta = (DisplayName = "Set Json Object"))
		static void Object_SetJsonObject(ULowEntryJsonObject* JsonObject, const FString& Key, ULowEntryJsonObject* Value);

	/**
	* Sets the value of the entry of the given key to a copy of the given Json Array.
	* A new entry will be created if no entry with the given key yet exists.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Object|Field", Meta = (DisplayName = "Set Json Array"))
		static void Object_SetJsonArray(ULowEntryJsonObject* JsonObject, const FString& Key, ULowEntryJsonArray* Value);

	/**
	* Sets the value of the entry of the given key to a copy of the value of the given Json Value.
	* A new entry will be created if no entry with the given key yet exists.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Object|Field", Meta = (DisplayName = "Set Json Value"))
		static void Object_SetJsonValue(ULowEntryJsonObject* JsonObject, const FString& Key, ULowEntryJsonValue* Value);


	/**
	* Returns the value and the type of the value, returns as a None value if no entry with the given key exists.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Object|Field", Meta = (DisplayName = "Get", ExpandEnumAsExecs = "Branch"))
		static void Object_Get(ULowEntryJsonObject* JsonObject, const FString& Key, ELowEntryJsonType& Branch, FString& ReturnStringValue, int32& ReturnIntegerValue, float& ReturnFloatValue, uint8& ReturnByteValue, bool& ReturnBooleanValue, ULowEntryJsonObject*& ReturnJsonObjectValue, ULowEntryJsonArray*& ReturnJsonArrayValue);

	/**
	* Shows you if an entry with the given key exists and if the value is null or not.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Object|Field", Meta = (DisplayName = "Get Null", ExpandEnumAsExecs = "Branch"))
		static void Object_GetNull(ULowEntryJsonObject* JsonObject, const FString& Key, ELowEntryJsonValueAndTypeFound& Branch);

	/**
	* Returns the value as a String if an entry with the given key exists and if the value is a String.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Object|Field", Meta = (DisplayName = "Get String", ExpandEnumAsExecs = "Branch"))
		static FString Object_GetString(ULowEntryJsonObject* JsonObject, const FString& Key, ELowEntryJsonValueAndTypeFound& Branch);

	/**
	* Returns the value as an Integer if an entry with the given key exists and if the value is a possible Integer (Float with less than 0.001 away from a whole number).
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Object|Field", Meta = (DisplayName = "Get Integer", ExpandEnumAsExecs = "Branch"))
		static int32 Object_GetInteger(ULowEntryJsonObject* JsonObject, const FString& Key, ELowEntryJsonValueAndTypeFound& Branch);

	/**
	* Returns the value as a Float if an entry with the given key exists and if the value is a Float.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Object|Field", Meta = (DisplayName = "Get Float", ExpandEnumAsExecs = "Branch"))
		static float Object_GetFloat(ULowEntryJsonObject* JsonObject, const FString& Key, ELowEntryJsonValueAndTypeFound& Branch);

	/**
	* Returns the value as a Byte if an entry with the given key exists and if the value is a possible Byte (Integer between 0 and 255).
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Object|Field", Meta = (DisplayName = "Get Byte", ExpandEnumAsExecs = "Branch"))
		static uint8 Object_GetByte(ULowEntryJsonObject* JsonObject, const FString& Key, ELowEntryJsonValueAndTypeFound& Branch);

	/**
	* Returns the value as a Boolean if an entry with the given key exists and if the value is a Boolean.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Object|Field", Meta = (DisplayName = "Get Boolean", ExpandEnumAsExecs = "Branch"))
		static bool Object_GetBoolean(ULowEntryJsonObject* JsonObject, const FString& Key, ELowEntryJsonValueAndTypeFound& Branch);

	/**
	* Returns the value as a copy of a Json Object if an entry with the given key exists and if the value is a Json Object.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Object|Field", Meta = (DisplayName = "Get Json Object", ExpandEnumAsExecs = "Branch"))
		static ULowEntryJsonObject* Object_GetJsonObject(ULowEntryJsonObject* JsonObject, const FString& Key, ELowEntryJsonValueAndTypeFound& Branch);

	/**
	* Returns the value as a copy of a Json Array if an entry with the given key exists and if the value is a Json Array.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Object|Field", Meta = (DisplayName = "Get Json Array", ExpandEnumAsExecs = "Branch"))
		static ULowEntryJsonArray* Object_GetJsonArray(ULowEntryJsonObject* JsonObject, const FString& Key, ELowEntryJsonValueAndTypeFound& Branch);

	/**
	* Returns the value as a copy of a Json Value if an entry with the given key exists.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Object|Field", Meta = (DisplayName = "Get Json Value", ExpandEnumAsExecs = "Branch"))
		static ULowEntryJsonValue* Object_GetJsonValue(ULowEntryJsonObject* JsonObject, const FString& Key, ELowEntryJsonValueFound& Branch);


	/**
	* Returns the value as a String, returns the Default value if not possible.
	*/
	UFUNCTION(BlueprintPure, Category = "Low Entry|Json|Object|Field", Meta = (DisplayName = "Get As String"))
		static FString Object_GetAsStringOrDefault(ULowEntryJsonObject* JsonObject, const FString& Key, const FString& Default);

	/**
	* Returns the value as an Integer, returns the Default value if not possible.
	*/
	UFUNCTION(BlueprintPure, Category = "Low Entry|Json|Object|Field", Meta = (DisplayName = "Get As Integer"))
		static int32 Object_GetAsIntegerOrDefault(ULowEntryJsonObject* JsonObject, const FString& Key, const int32 Default);

	/**
	* Returns the value as a Float, returns the Default value if not possible.
	*/
	UFUNCTION(BlueprintPure, Category = "Low Entry|Json|Object|Field", Meta = (DisplayName = "Get As Float"))
		static float Object_GetAsFloatOrDefault(ULowEntryJsonObject* JsonObject, const FString& Key, const float Default);

	/**
	* Returns the value as a Byte, returns the Default value if not possible.
	*/
	UFUNCTION(BlueprintPure, Category = "Low Entry|Json|Object|Field", Meta = (DisplayName = "Get As Byte"))
		static uint8 Object_GetAsByteOrDefault(ULowEntryJsonObject* JsonObject, const FString& Key, const uint8 Default);

	/**
	* Returns the value as a Boolean, returns the Default value if not possible.
	*/
	UFUNCTION(BlueprintPure, Category = "Low Entry|Json|Object|Field", Meta = (DisplayName = "Get As Boolean"))
		static bool Object_GetAsBooleanOrDefault(ULowEntryJsonObject* JsonObject, const FString& Key, const bool Default);

	/**
	* Returns the value as a copy of a Json Object, returns the Default value if not possible.
	*/
	UFUNCTION(BlueprintPure, Category = "Low Entry|Json|Object|Field", Meta = (DisplayName = "Get As Json Object"))
		static ULowEntryJsonObject* Object_GetAsJsonObjectOrDefault(ULowEntryJsonObject* JsonObject, const FString& Key, ULowEntryJsonObject* Default);

	/**
	* Returns the value as a copy of a Json Array, returns the Default value if not possible.
	*/
	UFUNCTION(BlueprintPure, Category = "Low Entry|Json|Object|Field", Meta = (DisplayName = "Get As Json Array"))
		static ULowEntryJsonArray* Object_GetAsJsonArrayOrDefault(ULowEntryJsonObject* JsonObject, const FString& Key, ULowEntryJsonArray* Default);

	/**
	* Returns the value as a copy of a Json Value, returns the Default value if not possible.
	*/
	UFUNCTION(BlueprintPure, Category = "Low Entry|Json|Object|Field", Meta = (DisplayName = "Get As Json Value"))
		static ULowEntryJsonValue* Object_GetAsJsonValueOrDefault(ULowEntryJsonObject* JsonObject, const FString& Key, ULowEntryJsonValue* Default);


	/**
	* Creates a String Array and fills it with the keys present in the given Json Object.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Object|Other", Meta = (DisplayName = "Get Keys"))
		static TArray<FString> Object_GetKeys(ULowEntryJsonObject* JsonObject);

	/**
	* Creates a Json Array and fills it with the values present in the given Json Object.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Object|Other", Meta = (DisplayName = "Get Values"))
		static ULowEntryJsonArray* Object_GetValues(ULowEntryJsonObject* JsonObject);

	/**
	* Returns the number of entries in the given Json Object.
	*/
	UFUNCTION(BlueprintPure, Category = "Low Entry|Json|Object|Other", Meta = (DisplayName = "Length"))
		static int32 Object_Length(ULowEntryJsonObject* JsonObject);


	/**
	* Adds a copy of the entries of the given Json Object to this Json Object.
	* This is basically the same as Merge, except that this changes this Json Object instead of creating a new one, which causes this function to be more efficient and faster.
	* When the appended Json Object contains a key that this Json Object already contains, the key in this Json Object will be set to the value that has been set in the given Json Object if OverrideDuplicates is true.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Object|Other", Meta = (DisplayName = "Append"))
		static void Object_Append(ULowEntryJsonObject* JsonObject, ULowEntryJsonObject* AppendJsonObject, const bool OverrideDuplicates = true);


	/**
	* Sorts the entries on the keys, A to Z if Reversed is false, Z to A if Reversed is true.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Object|Other", Meta = (DisplayName = "Sort"))
		static void Object_Sort(ULowEntryJsonObject* JsonObject, const bool Reversed = false);



public: // Object Entry
	/**
	* Creates a new Json Object Entry.
	*/
	UFUNCTION(BlueprintPure, Category = "Low Entry|Json|Object|Create Entry (Pure)", Meta = (DisplayName = "Create Json Object Entry (Json Value)"))
		static ULowEntryJsonObjectEntry* ObjectEntry_Pure_Create(const FString& Key, ULowEntryJsonValue* Value);

	/**
	* Creates a new Json Object Entry and sets its value to null.
	*/
	UFUNCTION(BlueprintPure, Category = "Low Entry|Json|Object|Create Entry (Pure)", Meta = (DisplayName = "Create Json Object Entry (Null)"))
		static ULowEntryJsonObjectEntry* ObjectEntry_Pure_CreateFromNull(const FString& Key);

	/**
	* Creates a new Json Object Entry and sets its value to the given String.
	*/
	UFUNCTION(BlueprintPure, Category = "Low Entry|Json|Object|Create Entry (Pure)", Meta = (DisplayName = "Create Json Object Entry (String)"))
		static ULowEntryJsonObjectEntry* ObjectEntry_Pure_CreateFromString(const FString& Key, const FString& Value);

	/**
	* Creates a new Json Object Entry and sets its value to the given Integer.
	*/
	UFUNCTION(BlueprintPure, Category = "Low Entry|Json|Object|Create Entry (Pure)", Meta = (DisplayName = "Create Json Object Entry (Integer)"))
		static ULowEntryJsonObjectEntry* ObjectEntry_Pure_CreateFromInteger(const FString& Key, const int32 Value);

	/**
	* Creates a new Json Object Entry and sets its value to the given Float.
	*/
	UFUNCTION(BlueprintPure, Category = "Low Entry|Json|Object|Create Entry (Pure)", Meta = (DisplayName = "Create Json Object Entry (Float)"))
		static ULowEntryJsonObjectEntry* ObjectEntry_Pure_CreateFromFloat(const FString& Key, const float Value);

	/**
	* Creates a new Json Object Entry and sets its value to the given Byte.
	*/
	UFUNCTION(BlueprintPure, Category = "Low Entry|Json|Object|Create Entry (Pure)", Meta = (DisplayName = "Create Json Object Entry (Byte)"))
		static ULowEntryJsonObjectEntry* ObjectEntry_Pure_CreateFromByte(const FString& Key, const uint8 Value);

	/**
	* Creates a new Json Object Entry and sets its value to the given Boolean.
	*/
	UFUNCTION(BlueprintPure, Category = "Low Entry|Json|Object|Create Entry (Pure)", Meta = (DisplayName = "Create Json Object Entry (Boolean)"))
		static ULowEntryJsonObjectEntry* ObjectEntry_Pure_CreateFromBoolean(const FString& Key, const bool Value);

	/**
	* Creates a new Json Object Entry and sets its value to a copy the given Json Object.
	*/
	UFUNCTION(BlueprintPure, Category = "Low Entry|Json|Object|Create Entry (Pure)", Meta = (DisplayName = "Create Json Object Entry (Json Object)"))
		static ULowEntryJsonObjectEntry* ObjectEntry_Pure_CreateFromJsonObject(const FString& Key, ULowEntryJsonObject* Value);

	/**
	* Creates a new Json Object Entry and sets its value to a copy the given Json Array.
	*/
	UFUNCTION(BlueprintPure, Category = "Low Entry|Json|Object|Create Entry (Pure)", Meta = (DisplayName = "Create Json Object Entry (Json Array)"))
		static ULowEntryJsonObjectEntry* ObjectEntry_Pure_CreateFromJsonArray(const FString& Key, ULowEntryJsonArray* Value);



public: // Object Iterator
	/**
	* Creates a new Json Object Iterator that will iterate over the keys and values of the given Json Object.
	*
	* The Json Object Iterator will only iterate over the keys and values present in the Json Object at the time of creating this Json Object Iterator.
	* It will always keep iterating over the same keys and values, even when the Json Object has been changed, deleted, cleared, etc.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Object|Iterator", Meta = (DisplayName = "Create Json Object Iterator"))
		static ULowEntryJsonObjectIterator* ObjectIterator_Create(ULowEntryJsonObject* JsonObject);

	/**
	* Creates a new Json Object Iterator that will iterate over the same keys and values as the given Json Object Iterator.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Object|Iterator", Meta = (DisplayName = "Clone Json Object Iterator"))
		static ULowEntryJsonObjectIterator* ObjectIterator_Clone(ULowEntryJsonObjectIterator* JsonObjectIterator);


	/**
	* If there is an entry with the given index, it will create and return a Json Value with its value set to a copy of the value of the entry found, it will also return the Json Object key it was assigned to during the creation of this Json Object Iterator.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Object|Iterator", Meta = (DisplayName = "Get", ExpandEnumAsExecs = "Branch"))
		static void ObjectIterator_Get(ULowEntryJsonObjectIterator* JsonObjectIterator, const int32 Index, FString& Key, ULowEntryJsonValue*& JsonValue, ELowEntryJsonValueFound& Branch);


	/**
	* Returns the String Array the given Json Object Iterator iterates over.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Object|Iterator", Meta = (DisplayName = "Get Keys"))
		static TArray<FString> ObjectIterator_GetKeys(ULowEntryJsonObjectIterator* JsonObjectIterator);

	/**
	* Returns the Json Array the given Json Object Iterator iterates over.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Object|Iterator", Meta = (DisplayName = "Get Values"))
		static ULowEntryJsonArray* ObjectIterator_GetValues(ULowEntryJsonObjectIterator* JsonObjectIterator);

	/**
	* Returns the number of entries the given Json Object Iterator iterates over.
	*/
	UFUNCTION(BlueprintPure, Category = "Low Entry|Json|Object|Iterator", Meta = (DisplayName = "Length"))
		static int32 ObjectIterator_Length(ULowEntryJsonObjectIterator* JsonObjectIterator);



public: // Array
	/**
	* Creates a new Json Array.
	*/
	/*UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Array|Create (Callable)", Meta = (DisplayName = "Create Json Array"))*/
		static ULowEntryJsonArray* Array_Create();

	/**
	* Creates a new Json Array and fills it with the given Strings.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Array|Create (Callable)", Meta = (DisplayName = "Create Json Array (String Array)"))
		static ULowEntryJsonArray* Array_CreateFromStringArray(const TArray<FString>& Value);

	/**
	* Creates a new Json Array and fills it with the given Integers.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Array|Create (Callable)", Meta = (DisplayName = "Create Json Array (Integer Array)"))
		static ULowEntryJsonArray* Array_CreateFromIntegerArray(const TArray<int32>& Value);

	/**
	* Creates a new Json Array and fills it with the given Floats.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Array|Create (Callable)", Meta = (DisplayName = "Create Json Array (Float Array)"))
		static ULowEntryJsonArray* Array_CreateFromFloatArray(const TArray<float>& Value);

	/**
	* Creates a new Json Array and fills it with the given Bytes.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Array|Create (Callable)", Meta = (DisplayName = "Create Json Array (Byte Array)"))
		static ULowEntryJsonArray* Array_CreateFromByteArray(const TArray<uint8>& Value);

	/**
	* Creates a new Json Array and fills it with the given Booleans.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Array|Create (Callable)", Meta = (DisplayName = "Create Json Array (Boolean Array)"))
		static ULowEntryJsonArray* Array_CreateFromBooleanArray(const TArray<bool>& Value);

	/**
	* Creates a new Json Array and fills it with copies of the given Json Objects.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Array|Create (Callable)", Meta = (DisplayName = "Create Json Array (Json Object Array)"))
		static ULowEntryJsonArray* Array_CreateFromJsonObjectArray(const TArray<ULowEntryJsonObject*>& Value);

	/**
	* Creates a new Json Array and fills it with copies of the given Json Arrays.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Array|Create (Callable)", Meta = (DisplayName = "Create Json Array (Json Array Array)"))
		static ULowEntryJsonArray* Array_CreateFromJsonArrayArray(const TArray<ULowEntryJsonArray*>& Value);

	/**
	* Creates a new Json Array and fills it with copies of the values of the given Json Values.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Array|Create (Callable)", Meta = (DisplayName = "Create Json Array (Json Value Array)"))
		static ULowEntryJsonArray* Array_CreateFromJsonValueArray(const TArray<ULowEntryJsonValue*>& Value);

	/**
	* Creates a new Json Array and fills it with a copy of the data from the given Json Array.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Array|Create (Callable)", Meta = (DisplayName = "Clone"))
		static ULowEntryJsonArray* Array_Clone(ULowEntryJsonArray* JsonArray);

	/**
	* Creates a new Json Array and fills it with a copy of the data from the two given Json Arrays.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Array|Create (Callable)", Meta = (DisplayName = "Merge"))
		static ULowEntryJsonArray* Array_Merge(ULowEntryJsonArray* JsonArray1, ULowEntryJsonArray* JsonArray2);


	/**
	* Creates a new Json Array.
	*/
	/*UFUNCTION(BlueprintPure, Category = "Low Entry|Json|Array|Create (Pure)", Meta = (DisplayName = "Create Json Array"))*/
		static ULowEntryJsonArray* Array_Pure_Create();

	/**
	* Creates a new Json Array and fills it with the given Strings.
	*/
	UFUNCTION(BlueprintPure, Category = "Low Entry|Json|Array|Create (Pure)", Meta = (DisplayName = "Create Json Array (String Array)"))
		static ULowEntryJsonArray* Array_Pure_CreateFromStringArray(const TArray<FString>& Value);

	/**
	* Creates a new Json Array and fills it with the given Integers.
	*/
	UFUNCTION(BlueprintPure, Category = "Low Entry|Json|Array|Create (Pure)", Meta = (DisplayName = "Create Json Array (Integer Array)"))
		static ULowEntryJsonArray* Array_Pure_CreateFromIntegerArray(const TArray<int32>& Value);

	/**
	* Creates a new Json Array and fills it with the given Floats.
	*/
	UFUNCTION(BlueprintPure, Category = "Low Entry|Json|Array|Create (Pure)", Meta = (DisplayName = "Create Json Array (Float Array)"))
		static ULowEntryJsonArray* Array_Pure_CreateFromFloatArray(const TArray<float>& Value);

	/**
	* Creates a new Json Array and fills it with the given Bytes.
	*/
	UFUNCTION(BlueprintPure, Category = "Low Entry|Json|Array|Create (Pure)", Meta = (DisplayName = "Create Json Array (Byte Array)"))
		static ULowEntryJsonArray* Array_Pure_CreateFromByteArray(const TArray<uint8>& Value);

	/**
	* Creates a new Json Array and fills it with the given Booleans.
	*/
	UFUNCTION(BlueprintPure, Category = "Low Entry|Json|Array|Create (Pure)", Meta = (DisplayName = "Create Json Array (Boolean Array)"))
		static ULowEntryJsonArray* Array_Pure_CreateFromBooleanArray(const TArray<bool>& Value);

	/**
	* Creates a new Json Array and fills it with copies of the given Json Objects.
	*/
	UFUNCTION(BlueprintPure, Category = "Low Entry|Json|Array|Create (Pure)", Meta = (DisplayName = "Create Json Array (Json Object Array)"))
		static ULowEntryJsonArray* Array_Pure_CreateFromJsonObjectArray(const TArray<ULowEntryJsonObject*>& Value);

	/**
	* Creates a new Json Array and fills it with copies of the given Json Arrays.
	*/
	UFUNCTION(BlueprintPure, Category = "Low Entry|Json|Array|Create (Pure)", Meta = (DisplayName = "Create Json Array (Json Array Array)"))
		static ULowEntryJsonArray* Array_Pure_CreateFromJsonArrayArray(const TArray<ULowEntryJsonArray*>& Value);

	/**
	* Creates a new Json Array and fills it with copies of the values of the given Json Values.
	*/
	UFUNCTION(BlueprintPure, Category = "Low Entry|Json|Array|Create (Pure)", Meta = (DisplayName = "Create Json Array (Json Value Array)"))
		static ULowEntryJsonArray* Array_Pure_CreateFromJsonValueArray(const TArray<ULowEntryJsonValue*>& Value);

	/**
	* Creates a new Json Array and fills it with a copy of the data from the given Json Array.
	*/
	UFUNCTION(BlueprintPure, Category = "Low Entry|Json|Array|Create (Pure)", Meta = (DisplayName = "Clone"))
		static ULowEntryJsonArray* Array_Pure_Clone(ULowEntryJsonArray* JsonArray);

	/**
	* Creates a new Json Array and fills it with a copy of the data from the two given Json Arrays.
	*/
	UFUNCTION(BlueprintPure, Category = "Low Entry|Json|Array|Create (Pure)", Meta = (DisplayName = "Merge"))
		static ULowEntryJsonArray* Array_Pure_Merge(ULowEntryJsonArray* JsonArray1, ULowEntryJsonArray* JsonArray2);


	/**
	* Creates a Json String from the given Json Array.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Json String|Stringify", Meta = (DisplayName = "To Json String (Json Array)"))
		static FString Array_ToJsonString(ULowEntryJsonArray* JsonArray, const bool PrettyPrint = true);

	/**
	* Creates a Json String from the given Json Array.
	*/
	UFUNCTION(BlueprintPure, Category = "Low Entry|Json|Json String|Stringify", Meta = (DisplayName = "To Json String (Json Array) (Pure)"))
		static FString Array_Pure_ToJsonString(ULowEntryJsonArray* JsonArray, const bool PrettyPrint = true);


	/**
	* Removes all entries from this Json Array, making it an empty Json Array.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Array|Other", Meta = (DisplayName = "Clear"))
		static void Array_Clear(ULowEntryJsonArray* JsonArray);

	/**
	* If the given index(es) exists, it or they will be removed from this Json Array, higher indexes will be reduced by the amount of entries deleted.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Array|Field", Meta = (DisplayName = "Unset"))
		static void Array_Unset(ULowEntryJsonArray* JsonArray, const int32 Index, const int32 Count = 1);

	/**
	* Returns true if this Json Array has a value set on the given index.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Array|Field", Meta = (DisplayName = "Is Set"))
		static bool Array_IsSet(ULowEntryJsonArray* JsonArray, const int32 Index);


	/**
	* Adds null.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Array|Field", Meta = (DisplayName = "Add Null"))
		static void Array_AddNull(ULowEntryJsonArray* JsonArray);

	/**
	* Adds the given String.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Array|Field", Meta = (DisplayName = "Add String"))
		static void Array_AddString(ULowEntryJsonArray* JsonArray, const FString& Value);

	/**
	* Adds the given Integer.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Array|Field", Meta = (DisplayName = "Add Integer"))
		static void Array_AddInteger(ULowEntryJsonArray* JsonArray, const int32 Value);

	/**
	* Adds the given Float.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Array|Field", Meta = (DisplayName = "Add Float"))
		static void Array_AddFloat(ULowEntryJsonArray* JsonArray, const float Value);

	/**
	* Adds the given Byte.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Array|Field", Meta = (DisplayName = "Add Byte"))
		static void Array_AddByte(ULowEntryJsonArray* JsonArray, const uint8 Value);

	/**
	* Adds the given Boolean.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Array|Field", Meta = (DisplayName = "Add Boolean"))
		static void Array_AddBoolean(ULowEntryJsonArray* JsonArray, const bool Value);

	/**
	* Adds a copy of the given Json Object.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Array|Field", Meta = (DisplayName = "Add Json Object"))
		static void Array_AddJsonObject(ULowEntryJsonArray* JsonArray, ULowEntryJsonObject* Value);

	/**
	* Adds a copy of the given Json Array.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Array|Field", Meta = (DisplayName = "Add Json Array"))
		static void Array_AddJsonArray(ULowEntryJsonArray* JsonArray, ULowEntryJsonArray* Value);

	/**
	* Adds a copy of the value of the given Json Value.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Array|Field", Meta = (DisplayName = "Add Json Value"))
		static void Array_AddJsonValue(ULowEntryJsonArray* JsonArray, ULowEntryJsonValue* Value);


	/**
	* Sets the value of the given index to null.
	* If the given index is outside of this Json Array, it will automatically expand this Json Array and set the values of all new entries to null.
	*
	* Note: This doesn't remove an entry because null is a valid value in Json, to remove an entry from a Json Array, use Unset.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Array|Field", Meta = (DisplayName = "Set Null"))
		static void Array_SetNull(ULowEntryJsonArray* JsonArray, const int32 Index);

	/**
	* Sets the value of the given index to the given String.
	* If the given index is outside of this Json Array, it will automatically expand this Json Array and set the values of all new entries to null.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Array|Field", Meta = (DisplayName = "Set String"))
		static void Array_SetString(ULowEntryJsonArray* JsonArray, const int32 Index, const FString& Value);

	/**
	* Sets the value of the given index to the given Integer.
	* If the given index is outside of this Json Array, it will automatically expand this Json Array and set the values of all new entries to null.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Array|Field", Meta = (DisplayName = "Set Integer"))
		static void Array_SetInteger(ULowEntryJsonArray* JsonArray, const int32 Index, const int32 Value);

	/**
	* Sets the value of the given index to the given Float.
	* If the given index is outside of this Json Array, it will automatically expand this Json Array and set the values of all new entries to null.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Array|Field", Meta = (DisplayName = "Set Float"))
		static void Array_SetFloat(ULowEntryJsonArray* JsonArray, const int32 Index, const float Value);

	/**
	* Sets the value of the given index to the given Byte.
	* If the given index is outside of this Json Array, it will automatically expand this Json Array and set the values of all new entries to null.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Array|Field", Meta = (DisplayName = "Set Byte"))
		static void Array_SetByte(ULowEntryJsonArray* JsonArray, const int32 Index, const uint8 Value);

	/**
	* Sets the value of the given index to the given Boolean.
	* If the given index is outside of this Json Array, it will automatically expand this Json Array and set the values of all new entries to null.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Array|Field", Meta = (DisplayName = "Set Boolean"))
		static void Array_SetBoolean(ULowEntryJsonArray* JsonArray, const int32 Index, const bool Value);

	/**
	* Sets the value of the given index to a copy of the given Json Object.
	* If the given index is outside of this Json Array, it will automatically expand this Json Array and set the values of all new entries to null.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Array|Field", Meta = (DisplayName = "Set Json Object"))
		static void Array_SetJsonObject(ULowEntryJsonArray* JsonArray, const int32 Index, ULowEntryJsonObject* Value);

	/**
	* Sets the value of the given index to a copy of the given Json Array.
	* If the given index is outside of this Json Array, it will automatically expand this Json Array and set the values of all new entries to null.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Array|Field", Meta = (DisplayName = "Set Json Array"))
		static void Array_SetJsonArray(ULowEntryJsonArray* JsonArray, const int32 Index, ULowEntryJsonArray* Value);

	/**
	* Sets the value of the given index to a copy of the value of the given Json Value.
	* If the given index is outside of this Json Array, it will automatically expand this Json Array and set the values of all new entries to null.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Array|Field", Meta = (DisplayName = "Set Json Value"))
		static void Array_SetJsonValue(ULowEntryJsonArray* JsonArray, const int32 Index, ULowEntryJsonValue* Value);


	/**
	* Inserts null before the given index, making this the new value at the given index, and increasing all indexes after this by one.
	* If the given index is outside of this Json Array, it will automatically expand this Json Array and set the values of all new entries to null.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Array|Field", Meta = (DisplayName = "Insert Null"))
		static void Array_InsertNull(ULowEntryJsonArray* JsonArray, const int32 Index);

	/**
	* Inserts the given String before the given index, making this the new value at the given index, and increasing all indexes after this by one.
	* If the given index is outside of this Json Array, it will automatically expand this Json Array and set the values of all new entries to null.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Array|Field", Meta = (DisplayName = "Insert String"))
		static void Array_InsertString(ULowEntryJsonArray* JsonArray, const int32 Index, const FString& Value);

	/**
	* Inserts the given Integer before the given index, making this the new value at the given index, and increasing all indexes after this by one.
	* If the given index is outside of this Json Array, it will automatically expand this Json Array and set the values of all new entries to null.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Array|Field", Meta = (DisplayName = "Insert Integer"))
		static void Array_InsertInteger(ULowEntryJsonArray* JsonArray, const int32 Index, const int32 Value);

	/**
	* Inserts the given Float before the given index, making this the new value at the given index, and increasing all indexes after this by one.
	* If the given index is outside of this Json Array, it will automatically expand this Json Array and set the values of all new entries to null.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Array|Field", Meta = (DisplayName = "Insert Float"))
		static void Array_InsertFloat(ULowEntryJsonArray* JsonArray, const int32 Index, const float Value);

	/**
	* Inserts the given Byte before the given index, making this the new value at the given index, and increasing all indexes after this by one.
	* If the given index is outside of this Json Array, it will automatically expand this Json Array and set the values of all new entries to null.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Array|Field", Meta = (DisplayName = "Insert Byte"))
		static void Array_InsertByte(ULowEntryJsonArray* JsonArray, const int32 Index, const uint8 Value);

	/**
	* Inserts the given Boolean before the given index, making this the new value at the given index, and increasing all indexes after this by one.
	* If the given index is outside of this Json Array, it will automatically expand this Json Array and set the values of all new entries to null.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Array|Field", Meta = (DisplayName = "Insert Boolean"))
		static void Array_InsertBoolean(ULowEntryJsonArray* JsonArray, const int32 Index, const bool Value);

	/**
	* Inserts a copy of the given Json Object before the given index, making this the new value at the given index, and increasing all indexes after this by one.
	* If the given index is outside of this Json Array, it will automatically expand this Json Array and set the values of all new entries to null.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Array|Field", Meta = (DisplayName = "Insert Json Object"))
		static void Array_InsertJsonObject(ULowEntryJsonArray* JsonArray, const int32 Index, ULowEntryJsonObject* Value);

	/**
	* Inserts a copy of the given Json Array before the given index, making this the new value at the given index, and increasing all indexes after this by one.
	* If the given index is outside of this Json Array, it will automatically expand this Json Array and set the values of all new entries to null.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Array|Field", Meta = (DisplayName = "Insert Json Array"))
		static void Array_InsertJsonArray(ULowEntryJsonArray* JsonArray, const int32 Index, ULowEntryJsonArray* Value);

	/**
	* Inserts a copy of the value of the given Json Value before the given index, making this the new value at the given index, and increasing all indexes after this by one.
	* If the given index is outside of this Json Array, it will automatically expand this Json Array and set the values of all new entries to null.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Array|Field", Meta = (DisplayName = "Insert Json Value"))
		static void Array_InsertJsonValue(ULowEntryJsonArray* JsonArray, const int32 Index, ULowEntryJsonValue* Value);


	/**
	* Returns the value and the type of the value, returns as a None value if no entry with the given key exists.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Array|Field", Meta = (DisplayName = "Get", ExpandEnumAsExecs = "Branch"))
		static void Array_Get(ULowEntryJsonArray* JsonArray, const int32 Index, ELowEntryJsonType& Branch, FString& ReturnStringValue, int32& ReturnIntegerValue, float& ReturnFloatValue, uint8& ReturnByteValue, bool& ReturnBooleanValue, ULowEntryJsonObject*& ReturnJsonObjectValue, ULowEntryJsonArray*& ReturnJsonArrayValue);

	/**
	* Shows you if an entry with the given key exists and if the value is null or not.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Array|Field", Meta = (DisplayName = "Get Null", ExpandEnumAsExecs = "Branch"))
		static void Array_GetNull(ULowEntryJsonArray* JsonArray, const int32 Index, ELowEntryJsonValueAndTypeFound& Branch);

	/**
	* Returns the value as a String if an entry with the given key exists and if the value is a String.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Array|Field", Meta = (DisplayName = "Get String", ExpandEnumAsExecs = "Branch"))
		static FString Array_GetString(ULowEntryJsonArray* JsonArray, const int32 Index, ELowEntryJsonValueAndTypeFound& Branch);

	/**
	* Returns the value as an Integer if an entry with the given key exists and if the value is a possible Integer (Float with less than 0.001 away from a whole number).
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Array|Field", Meta = (DisplayName = "Get Integer", ExpandEnumAsExecs = "Branch"))
		static int32 Array_GetInteger(ULowEntryJsonArray* JsonArray, const int32 Index, ELowEntryJsonValueAndTypeFound& Branch);

	/**
	* Returns the value as a Float if an entry with the given key exists and if the value is a Float.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Array|Field", Meta = (DisplayName = "Get Float", ExpandEnumAsExecs = "Branch"))
		static float Array_GetFloat(ULowEntryJsonArray* JsonArray, const int32 Index, ELowEntryJsonValueAndTypeFound& Branch);

	/**
	* Returns the value as a Byte if an entry with the given key exists and if the value is a possible Byte (Integer between 0 and 255).
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Array|Field", Meta = (DisplayName = "Get Byte", ExpandEnumAsExecs = "Branch"))
		static uint8 Array_GetByte(ULowEntryJsonArray* JsonArray, const int32 Index, ELowEntryJsonValueAndTypeFound& Branch);

	/**
	* Returns the value as a Boolean if an entry with the given key exists and if the value is a Boolean.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Array|Field", Meta = (DisplayName = "Get Boolean", ExpandEnumAsExecs = "Branch"))
		static bool Array_GetBoolean(ULowEntryJsonArray* JsonArray, const int32 Index, ELowEntryJsonValueAndTypeFound& Branch);

	/**
	* Returns the value as a copy of a Json Object if an entry with the given key exists and if the value is a Json Object.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Array|Field", Meta = (DisplayName = "Get Json Object", ExpandEnumAsExecs = "Branch"))
		static ULowEntryJsonObject* Array_GetJsonObject(ULowEntryJsonArray* JsonArray, const int32 Index, ELowEntryJsonValueAndTypeFound& Branch);

	/**
	* Returns the value as a copy of a Json Array if an entry with the given key exists and if the value is a Json Array.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Array|Field", Meta = (DisplayName = "Get Json Array", ExpandEnumAsExecs = "Branch"))
		static ULowEntryJsonArray* Array_GetJsonArray(ULowEntryJsonArray* JsonArray, const int32 Index, ELowEntryJsonValueAndTypeFound& Branch);

	/**
	* Returns the value as a copy of a Json Value if an entry with the given index exists.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Array|Field", Meta = (DisplayName = "Get Json Value", ExpandEnumAsExecs = "Branch"))
		static ULowEntryJsonValue* Array_GetJsonValue(ULowEntryJsonArray* JsonArray, const int32 Index, ELowEntryJsonValueFound& Branch);


	/**
	* Returns the value as a String, returns the Default value if not possible.
	*/
	UFUNCTION(BlueprintPure, Category = "Low Entry|Json|Array|Field", Meta = (DisplayName = "Get As String"))
		static FString Array_GetAsStringOrDefault(ULowEntryJsonArray* JsonArray, const int32 Index, const FString& Default);

	/**
	* Returns the value as an Integer, returns the Default value if not possible.
	*/
	UFUNCTION(BlueprintPure, Category = "Low Entry|Json|Array|Field", Meta = (DisplayName = "Get As Integer"))
		static int32 Array_GetAsIntegerOrDefault(ULowEntryJsonArray* JsonArray, const int32 Index, const int32 Default);

	/**
	* Returns the value as a Float, returns the Default value if not possible.
	*/
	UFUNCTION(BlueprintPure, Category = "Low Entry|Json|Array|Field", Meta = (DisplayName = "Get As Float"))
		static float Array_GetAsFloatOrDefault(ULowEntryJsonArray* JsonArray, const int32 Index, const float Default);

	/**
	* Returns the value as a Byte, returns the Default value if not possible.
	*/
	UFUNCTION(BlueprintPure, Category = "Low Entry|Json|Array|Field", Meta = (DisplayName = "Get As Byte"))
		static uint8 Array_GetAsByteOrDefault(ULowEntryJsonArray* JsonArray, const int32 Index, const uint8 Default);

	/**
	* Returns the value as a Boolean, returns the Default value if not possible.
	*/
	UFUNCTION(BlueprintPure, Category = "Low Entry|Json|Array|Field", Meta = (DisplayName = "Get As Boolean"))
		static bool Array_GetAsBooleanOrDefault(ULowEntryJsonArray* JsonArray, const int32 Index, const bool Default);

	/**
	* Returns the value as a copy of a Json Object, returns the Default value if not possible.
	*/
	UFUNCTION(BlueprintPure, Category = "Low Entry|Json|Array|Field", Meta = (DisplayName = "Get As Json Object"))
		static ULowEntryJsonObject* Array_GetAsJsonObjectOrDefault(ULowEntryJsonArray* JsonArray, const int32 Index, ULowEntryJsonObject* Default);

	/**
	* Returns the value as a copy of a Json Array, returns the Default value if not possible.
	*/
	UFUNCTION(BlueprintPure, Category = "Low Entry|Json|Array|Field", Meta = (DisplayName = "Get As Json Array"))
		static ULowEntryJsonArray* Array_GetAsJsonArrayOrDefault(ULowEntryJsonArray* JsonArray, const int32 Index, ULowEntryJsonArray* Default);

	/**
	* Returns the value as a copy of a Json Value, returns the Default value if not possible.
	*/
	UFUNCTION(BlueprintPure, Category = "Low Entry|Json|Array|Field", Meta = (DisplayName = "Get As Json Value"))
		static ULowEntryJsonValue* Array_GetAsJsonValueOrDefault(ULowEntryJsonArray* JsonArray, const int32 Index, ULowEntryJsonValue* Default);


	/**
	* Returns the number of entries in the given Json Array.
	*/
	UFUNCTION(BlueprintPure, Category = "Low Entry|Json|Array|Other", Meta = (DisplayName = "Length"))
		static int32 Array_Length(ULowEntryJsonArray* JsonArray);


	/**
	* Expands or shortens this Json Array to the given Size, new entries will be set to null.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Array|Other", Meta = (DisplayName = "Resize"))
		static void Array_Resize(ULowEntryJsonArray* JsonArray, const int32 Size);

	/**
	* Expands this Json Array to the given Size, new entries will be set to null.
	* Unlike Resize, it will not shorten this Json Array if the current size is larger than the given size.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Array|Other", Meta = (DisplayName = "Expand"))
		static void Array_Expand(ULowEntryJsonArray* JsonArray, const int32 Size);

	/**
	* Shortens this Json Array to the given Size.
	* Unlike Resize, it will not expand this Json Array if the current size is smaller than the given size.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Array|Other", Meta = (DisplayName = "Shorten"))
		static void Array_Shorten(ULowEntryJsonArray* JsonArray, const int32 Size);


	/**
	* Adds a copy of the entries of the given Json Array to this Json Array.
	* This is basically the same as Merge, except that this changes this Json Array instead of creating a new one, which causes this function to be more efficient and faster.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Array|Other", Meta = (DisplayName = "Append"))
		static void Array_Append(ULowEntryJsonArray* JsonArray, ULowEntryJsonArray* AppendJsonArray);



public: // Value
	/**
	* Creates a new Json Value and sets its value to null.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Value|Create (Callable)", Meta = (DisplayName = "Create Json Value"))
		static ULowEntryJsonValue* Value_Create();

	/**
	* Creates a new Json Value and sets its value to null.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Value|Create (Callable)", Meta = (DisplayName = "Create Json Value (Null)"))
		static ULowEntryJsonValue* Value_CreateFromNull();

	/**
	* Creates a new Json Value and sets its value to the given String.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Value|Create (Callable)", Meta = (DisplayName = "Create Json Value (String)"))
		static ULowEntryJsonValue* Value_CreateFromString(const FString& Value);

	/**
	* Creates a new Json Value and sets its value to the given Integer.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Value|Create (Callable)", Meta = (DisplayName = "Create Json Value (Integer)"))
		static ULowEntryJsonValue* Value_CreateFromInteger(const int32 Value);

	/**
	* Creates a new Json Value and sets its value to the given Float.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Value|Create (Callable)", Meta = (DisplayName = "Create Json Value (Float)"))
		static ULowEntryJsonValue* Value_CreateFromFloat(const float Value);

	/**
	* Creates a new Json Value and sets its value to the given Byte.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Value|Create (Callable)", Meta = (DisplayName = "Create Json Value (Byte)"))
		static ULowEntryJsonValue* Value_CreateFromByte(const uint8 Value);

	/**
	* Creates a new Json Value and sets its value to the given Boolean.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Value|Create (Callable)", Meta = (DisplayName = "Create Json Value (Boolean)"))
		static ULowEntryJsonValue* Value_CreateFromBoolean(const bool Value);

	/**
	* Creates a new Json Value and sets its value to a copy the given Json Object.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Value|Create (Callable)", Meta = (DisplayName = "Create Json Value (Json Object)"))
		static ULowEntryJsonValue* Value_CreateFromJsonObject(ULowEntryJsonObject* Value);

	/**
	* Creates a new Json Value and sets its value to a copy the given Json Array.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Value|Create (Callable)", Meta = (DisplayName = "Create Json Value (Json Array)"))
		static ULowEntryJsonValue* Value_CreateFromJsonArray(ULowEntryJsonArray* Value);

	/**
	* Creates a new Json Value and sets its value to a copy of the value of the given Json Value.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Value|Create (Callable)", Meta = (DisplayName = "Clone"))
		static ULowEntryJsonValue* Value_Clone(ULowEntryJsonValue* JsonValue);


	/**
	* Creates a new Json Value and sets its value to null.
	*/
	UFUNCTION(BlueprintPure, Category = "Low Entry|Json|Value|Create (Pure)", Meta = (DisplayName = "Create Json Value"))
		static ULowEntryJsonValue* Value_Pure_Create();

	/**
	* Creates a new Json Value and sets its value to null.
	*/
	UFUNCTION(BlueprintPure, Category = "Low Entry|Json|Value|Create (Pure)", Meta = (DisplayName = "Create Json Value (Null)"))
		static ULowEntryJsonValue* Value_Pure_CreateFromNull();

	/**
	* Creates a new Json Value and sets its value to the given String.
	*/
	UFUNCTION(BlueprintPure, Category = "Low Entry|Json|Value|Create (Pure)", Meta = (DisplayName = "Create Json Value (String)"))
		static ULowEntryJsonValue* Value_Pure_CreateFromString(const FString& Value);

	/**
	* Creates a new Json Value and sets its value to the given Integer.
	*/
	UFUNCTION(BlueprintPure, Category = "Low Entry|Json|Value|Create (Pure)", Meta = (DisplayName = "Create Json Value (Integer)"))
		static ULowEntryJsonValue* Value_Pure_CreateFromInteger(const int32 Value);

	/**
	* Creates a new Json Value and sets its value to the given Float.
	*/
	UFUNCTION(BlueprintPure, Category = "Low Entry|Json|Value|Create (Pure)", Meta = (DisplayName = "Create Json Value (Float)"))
		static ULowEntryJsonValue* Value_Pure_CreateFromFloat(const float Value);

	/**
	* Creates a new Json Value and sets its value to the given Byte.
	*/
	UFUNCTION(BlueprintPure, Category = "Low Entry|Json|Value|Create (Pure)", Meta = (DisplayName = "Create Json Value (Byte)"))
		static ULowEntryJsonValue* Value_Pure_CreateFromByte(const uint8 Value);

	/**
	* Creates a new Json Value and sets its value to the given Boolean.
	*/
	UFUNCTION(BlueprintPure, Category = "Low Entry|Json|Value|Create (Pure)", Meta = (DisplayName = "Create Json Value (Boolean)"))
		static ULowEntryJsonValue* Value_Pure_CreateFromBoolean(const bool Value);

	/**
	* Creates a new Json Value and sets its value to a copy the given Json Object.
	*/
	UFUNCTION(BlueprintPure, Category = "Low Entry|Json|Value|Create (Pure)", Meta = (DisplayName = "Create Json Value (Json Object)"))
		static ULowEntryJsonValue* Value_Pure_CreateFromJsonObject(ULowEntryJsonObject* Value);

	/**
	* Creates a new Json Value and sets its value to a copy the given Json Array.
	*/
	UFUNCTION(BlueprintPure, Category = "Low Entry|Json|Value|Create (Pure)", Meta = (DisplayName = "Create Json Value (Json Array)"))
		static ULowEntryJsonValue* Value_Pure_CreateFromJsonArray(ULowEntryJsonArray* Value);

	/**
	* Creates a new Json Value and sets its value to a copy of the value of the given Json Value.
	*/
	UFUNCTION(BlueprintPure, Category = "Low Entry|Json|Value|Create (Pure)", Meta = (DisplayName = "Clone"))
		static ULowEntryJsonValue* Value_Pure_Clone(ULowEntryJsonValue* JsonValue);


	/**
	* Sets the value of this Json Value to a copy of the value of the given Json Value.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Value|Value", Meta = (DisplayName = "Set Json Value"))
		static void Value_Set(ULowEntryJsonValue* JsonValue, ULowEntryJsonValue* Value);

	/**
	* Sets the value of this Json Value to null.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Value|Value", Meta = (DisplayName = "Set Null"))
		static void Value_SetNull(ULowEntryJsonValue* JsonValue);

	/**
	* Sets the value of this Json Value to the given String.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Value|Value", Meta = (DisplayName = "Set String"))
		static void Value_SetString(ULowEntryJsonValue* JsonValue, const FString& Value);

	/**
	* Sets the value of this Json Value to the given Integer.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Value|Value", Meta = (DisplayName = "Set Integer"))
		static void Value_SetInteger(ULowEntryJsonValue* JsonValue, const int32 Value);

	/**
	* Sets the value of this Json Value to the given Float.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Value|Value", Meta = (DisplayName = "Set Float"))
		static void Value_SetFloat(ULowEntryJsonValue* JsonValue, const float Value);

	/**
	* Sets the value of this Json Value to the given Byte.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Value|Value", Meta = (DisplayName = "Set Byte"))
		static void Value_SetByte(ULowEntryJsonValue* JsonValue, const uint8 Value);

	/**
	* Sets the value of this Json Value to the given Boolean.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Value|Value", Meta = (DisplayName = "Set Boolean"))
		static void Value_SetBoolean(ULowEntryJsonValue* JsonValue, const bool Value);

	/**
	* Sets the value of this Json Value to a copy of the given Json Object.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Value|Value", Meta = (DisplayName = "Set Json Object"))
		static void Value_SetJsonObject(ULowEntryJsonValue* JsonValue, ULowEntryJsonObject* Value);

	/**
	* Sets the value of this Json Value to a copy of the given Json Array.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Value|Value", Meta = (DisplayName = "Set Json Array"))
		static void Value_SetJsonArray(ULowEntryJsonValue* JsonValue, ULowEntryJsonArray* Value);


	/**
	* Returns the value and the type of the value.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Value|Value", Meta = (DisplayName = "Get", ExpandEnumAsExecs = "Branch"))
		static void Value_Get(ULowEntryJsonValue* JsonValue, ELowEntryJsonType& Branch, FString& ReturnStringValue, int32& ReturnIntegerValue, float& ReturnFloatValue, uint8& ReturnByteValue, bool& ReturnBooleanValue, ULowEntryJsonObject*& ReturnJsonObjectValue, ULowEntryJsonArray*& ReturnJsonArrayValue);

	/**
	* Shows you if the value is null or not.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Value|Value", Meta = (DisplayName = "Get Null", ExpandEnumAsExecs = "Branch"))
		static void Value_GetNull(ULowEntryJsonValue* JsonValue, ELowEntryJsonTypeFound& Branch);

	/**
	* Returns the value as a String if the value is a String.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Value|Value", Meta = (DisplayName = "Get String", ExpandEnumAsExecs = "Branch"))
		static FString Value_GetString(ULowEntryJsonValue* JsonValue, ELowEntryJsonTypeFound& Branch);

	/**
	* Returns the value as an Integer if the value is a possible Integer (Float with less than 0.001 away from a whole number).
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Value|Value", Meta = (DisplayName = "Get Integer", ExpandEnumAsExecs = "Branch"))
		static int32 Value_GetInteger(ULowEntryJsonValue* JsonValue, ELowEntryJsonTypeFound& Branch);

	/**
	* Returns the value as a Float if the value is a Float.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Value|Value", Meta = (DisplayName = "Get Float", ExpandEnumAsExecs = "Branch"))
		static float Value_GetFloat(ULowEntryJsonValue* JsonValue, ELowEntryJsonTypeFound& Branch);

	/**
	* Returns the value as a Byte if the value is a possible Byte (Integer between 0 and 255).
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Value|Value", Meta = (DisplayName = "Get Byte", ExpandEnumAsExecs = "Branch"))
		static uint8 Value_GetByte(ULowEntryJsonValue* JsonValue, ELowEntryJsonTypeFound& Branch);

	/**
	* Returns the value as a Boolean if the value is a Boolean.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Value|Value", Meta = (DisplayName = "Get Boolean", ExpandEnumAsExecs = "Branch"))
		static bool Value_GetBoolean(ULowEntryJsonValue* JsonValue, ELowEntryJsonTypeFound& Branch);

	/**
	* Returns the value as a copy of a Json Object if the value is a Json Object.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Value|Value", Meta = (DisplayName = "Get Json Object", ExpandEnumAsExecs = "Branch"))
		static ULowEntryJsonObject* Value_GetJsonObject(ULowEntryJsonValue* JsonValue, ELowEntryJsonTypeFound& Branch);

	/**
	* Returns the value as a copy of a Json Array if the value is a Json Array.
	*/
	UFUNCTION(BlueprintCallable, Category = "Low Entry|Json|Value|Value", Meta = (DisplayName = "Get Json Array", ExpandEnumAsExecs = "Branch"))
		static ULowEntryJsonArray* Value_GetJsonArray(ULowEntryJsonValue* JsonValue, ELowEntryJsonTypeFound& Branch);


	/**
	* Returns the value as a String, returns the Default value if not possible.
	*/
	UFUNCTION(BlueprintPure, Category = "Low Entry|Json|Value|Value", Meta = (DisplayName = "Get As String"))
		static FString Value_GetAsStringOrDefault(ULowEntryJsonValue* JsonValue, const FString& Default);

	/**
	* Returns the value as an Integer, returns the Default value if not possible.
	*/
	UFUNCTION(BlueprintPure, Category = "Low Entry|Json|Value|Value", Meta = (DisplayName = "Get As Integer"))
		static int32 Value_GetAsIntegerOrDefault(ULowEntryJsonValue* JsonValue, const int32 Default);

	/**
	* Returns the value as a Float, returns the Default value if not possible.
	*/
	UFUNCTION(BlueprintPure, Category = "Low Entry|Json|Value|Value", Meta = (DisplayName = "Get As Float"))
		static float Value_GetAsFloatOrDefault(ULowEntryJsonValue* JsonValue, const float Default);

	/**
	* Returns the value as a Byte, returns the Default value if not possible.
	*/
	UFUNCTION(BlueprintPure, Category = "Low Entry|Json|Value|Value", Meta = (DisplayName = "Get As Byte"))
		static uint8 Value_GetAsByteOrDefault(ULowEntryJsonValue* JsonValue, const uint8 Default);

	/**
	* Returns the value as a Boolean, returns the Default value if not possible.
	*/
	UFUNCTION(BlueprintPure, Category = "Low Entry|Json|Value|Value", Meta = (DisplayName = "Get As Boolean"))
		static bool Value_GetAsBooleanOrDefault(ULowEntryJsonValue* JsonValue, const bool Default);

	/**
	* Returns the value as a copy of a Json Object, returns the Default value if not possible.
	*/
	UFUNCTION(BlueprintPure, Category = "Low Entry|Json|Value|Value", Meta = (DisplayName = "Get As Json Object"))
		static ULowEntryJsonObject* Value_GetAsJsonObjectOrDefault(ULowEntryJsonValue* JsonValue, ULowEntryJsonObject* Default);

	/**
	* Returns the value as a copy of a Json Array, returns the Default value if not possible.
	*/
	UFUNCTION(BlueprintPure, Category = "Low Entry|Json|Value|Value", Meta = (DisplayName = "Get As Json Array"))
		static ULowEntryJsonArray* Value_GetAsJsonArrayOrDefault(ULowEntryJsonValue* JsonValue, ULowEntryJsonArray* Default);
};
