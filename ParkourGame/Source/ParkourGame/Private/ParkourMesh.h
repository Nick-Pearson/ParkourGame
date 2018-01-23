#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "ParkourMesh.generated.h"

class UParkourInteractiveZone;

UCLASS()
class PARKOURGAME_API AParkourMesh : public AStaticMeshActor
{
	GENERATED_BODY()

public:

	AParkourMesh();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ParkourMesh")
	UParkourInteractiveZone* InteractiveZonePointer;
};
