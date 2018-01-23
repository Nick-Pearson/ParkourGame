#pragma once


#include "CoreMinimal.h"

#include "ELowEntryJsonValueAndTypeFound.generated.h"


UENUM()
enum class ELowEntryJsonValueAndTypeFound : uint8
{
	Success = 0,
	NotFound = 1,
	WrongType = 2
};
