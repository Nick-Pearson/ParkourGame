#include "SimpleSpringSystem.h"

void USimpleSpringSystem::Tick(float DeltaSeconds)
{
	// F = -kx - bv
	const float SpringForce = (SpringConstant * SpringCompression * -1) - (SpringDampening * SpringVelcoity);

	// a = F / m
	SpringVelcoity += SpringForce / Mass;

	// update compression
	SpringCompression += SpringVelcoity * DeltaSeconds;
}

void USimpleSpringSystem::AddVelocity(float VelocityChange)
{
	SpringVelcoity += VelocityChange;
}
