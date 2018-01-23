#include "ParkourMesh.h"
#include "ParkourInteractiveZone.h"
#include "Components/SphereComponent.h"

AParkourMesh::AParkourMesh()
{
	InteractiveZonePointer = CreateDefaultSubobject<UParkourInteractiveZone>("InteractiveZone");

	UStaticMeshComponent* MeshComp = GetStaticMeshComponent();
	check(MeshComp);

	MeshComp->bGenerateOverlapEvents = true;
}


