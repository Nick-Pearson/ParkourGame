// Fill out your copyright notice in the Description page of Project Settings.

#include "ParkourMesh.h"
#include "ParkourInteractiveZone.h"
#include "Components/SphereComponent.h"

AParkourMesh::AParkourMesh()
{
	InteractiveZonePointer = CreateDefaultSubobject<UParkourInteractiveZone>("InteractiveZone");
	ParkourCollision = CreateDefaultSubobject<USphereComponent>("CollisionRadius");
	ParkourCollision->SetSphereRadius(35.0f);
	ParkourCollision->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	ParkourCollision->SetRelativeLocation(FVector::ZeroVector);
}


