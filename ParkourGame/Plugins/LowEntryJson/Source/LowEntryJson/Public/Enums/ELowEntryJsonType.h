#pragma once


#include "CoreMinimal.h"

#include "ELowEntryJsonType.generated.h"


UENUM()
enum class ELowEntryJsonType : uint8
{
	None = 0,
	Null = 1,
	String = 2,
	Float = 3,
	IntegerOrFloat = 4,
	ByteOrIntegerOrFloat = 5,
	Boolean = 6,
	JsonObject = 7,
	JsonArray = 8
};
