#include "ParkourMovementComponent.h"

float UParkourMovementComponent::GetGravityZ() const
{
	const float SuperVal = Super::GetGravityZ();
	return bEnableGravity ? SuperVal : 0.0f;
}
