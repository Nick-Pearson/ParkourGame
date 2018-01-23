#include "SpringSystem.h"

void USpringSystem::Initialise(const FVector& inPoint1, const FVector& inPoint2)
{
	Point1 = inPoint1;
	Point2 = inPoint2;

	OldPoint2 = Point2;
	SpringForce = FVector::ZeroVector;
}

void USpringSystem::Tick(float DeltaTime)
{
	//calculate the point velocity based on the previous ticks value
	const FVector PointVelocity = (Point2 - OldPoint2) / DeltaTime;
	OldPoint2 = Point2;

	// update the spring force
	const FVector SpringDir = Point2 - Point1;
	const float SpringLength = SpringDir.Size();

	// F = -k(|x| - d)(x/|x|) - bv
	// https://gafferongames.com/post/spring_physics/
	SpringForce = (-1.0f * SpringConstant * (SpringLength - RestingSpringExtension) * (SpringDir / SpringLength)) - 
					(SpringDampening * PointVelocity);
}

FVector USpringSystem::GetSpringForce() const
{
	return SpringForce;
}