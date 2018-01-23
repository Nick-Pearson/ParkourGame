#pragma once


#include "CoreMinimal.h"

#include "ELowEntryJsonTypeFound.generated.h"


UENUM()
enum class ELowEntryJsonTypeFound : uint8
{
	Success = 0,
	WrongType = 1
};
