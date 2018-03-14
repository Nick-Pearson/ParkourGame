#include "PushSpringSystem.h"
#include "DrawDebugHelpers.h"

void UPushSpringSystem::Initialise(const FVector& inPoint1, const FVector& inPoint2)
{
	Point1 = inPoint1;
	Point2 = inPoint2;
}

FVector UPushSpringSystem::GetSpringForce(const FVector& newPoint2) const
{
	float pForce = FVector::DistSquaredXY(Point1, Point2);
	float nForce = FVector::DistSquaredXY(Point1, newPoint2);
	FVector Output = (Point1 - Point2);
	Output.SetComponentForAxis(EAxis::Y, 45.f);
	Output.Normalize();
	return FVector::UpVector * 200;
}