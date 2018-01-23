#pragma once


#include "CoreMinimal.h"

#include "ELowEntryJsonValueFound.generated.h"


UENUM()
enum class ELowEntryJsonValueFound : uint8
{
	Found = 0,
	NotFound = 1
};
