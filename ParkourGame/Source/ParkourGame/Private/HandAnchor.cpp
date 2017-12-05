// Fill out your copyright notice in the Description page of Project Settings.

#include "HandAnchor.h"


// Sets default values for this component's properties
UHandAnchor::UHandAnchor()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UHandAnchor::BeginPlay()
{
	Super::BeginPlay();

	bAbsoluteLocation = true;
	bAbsoluteRotation = true;
	bAbsoluteScale = true;
}


// Called every frame
void UHandAnchor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

