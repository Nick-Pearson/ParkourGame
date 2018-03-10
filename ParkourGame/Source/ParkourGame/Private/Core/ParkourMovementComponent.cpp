#include "ParkourMovementComponent.h"

float UParkourMovementComponent::GetGravityZ() const
{
	const float SuperVal = Super::GetGravityZ();
	return bEnableGravity ? SuperVal : 0.0f;
}

void UParkourMovementComponent::SetMaxAcceleration(float NewMaxAcceleration)
{
  MaxAcceleration = NewMaxAcceleration;
}
