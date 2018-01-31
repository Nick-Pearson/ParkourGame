#pragma once

#include "Engine/DataTable.h"
#include "FootstepAudioTableRow.generated.h"


USTRUCT(BlueprintType)
struct FFootstepAudioTableRow : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "FootstepAudio")
	TArray<USoundWave*> LeftFoot;

	UPROPERTY(EditAnywhere, Category = "FootstepAudio")
	TArray<USoundWave*> RightFoot;
};