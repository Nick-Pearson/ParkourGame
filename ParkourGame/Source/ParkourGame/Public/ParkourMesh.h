// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "ParkourMesh.generated.h"

/**
 * 
 */
class UParkourInteractiveZone;
class USphereComponent;
UCLASS()
class PARKOURGAME_API AParkourMesh : public AStaticMeshActor
{
	GENERATED_BODY()


	AParkourMesh();
	public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UParkourInteractiveZone* InteractiveZonePointer;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USphereComponent* ParkourCollision;
	
};
