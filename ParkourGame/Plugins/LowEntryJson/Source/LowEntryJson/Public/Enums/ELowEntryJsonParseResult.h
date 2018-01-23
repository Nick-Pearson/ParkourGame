#pragma once


#include "CoreMinimal.h"

#include "ELowEntryJsonParseResult.generated.h"


UENUM()
enum class ELowEntryJsonParseResult : uint8
{
	FoundObject = 0,
	FoundArray = 1,
	StringIsEmpty = 2,
	UnableToParse = 3
};
