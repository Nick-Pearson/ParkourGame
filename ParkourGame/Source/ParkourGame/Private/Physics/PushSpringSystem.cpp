#include "PushSpringSystem.h"
#include "DrawDebugHelpers.h"

void UPushSpringSystem::CalculatePoint2()
{

	FVector Path = Point3 - Point1;
	FVector Direction;
	float Length;
	Path.ToDirectionAndLength(Direction, Length);
	if (Length > 100.f)
		Length = 100.f;
	
	Direction *= Length;
	Point2 = Point1 + Direction;
}

void UPushSpringSystem::Initialise(const FVector& start, const FVector& end, AActor* ActorToIgnore)
{
	Point3 = start;
	SpringForce = FVector::ZeroVector;

	TArray<TEnumAsByte<EObjectTypeQuery>> TraceObjectTypes;

	TraceObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic));

	FCollisionQueryParams Params(FName(TEXT("Push Trace")), true, ActorToIgnore);

	FHitResult HitData(ForceInit);

	GetWorld()->LineTraceSingleByObjectType(
		HitData,		//result
		start,	//start
		end, //end
		FCollisionObjectQueryParams(TraceObjectTypes)
	);

	if (HitData.GetActor())
		Point2 = HitData.ImpactPoint;

	CalculatePoint2();
	DrawDebugPoint(
		GetWorld(),
		Point2,
		10,  					//size
		FColor(0, 255, 0),  //pink
		true,  				//persistent (never goes away)
		4.0 					//point leaves a trail on moving object
	);
	OldPoint2 = Point2;
}

void UPushSpringSystem::Tick(float DeltaTime)
{
	CalculatePoint2();
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

float UPushSpringSystem::GetSpringForce() const
{
	const FVector SpringDir = Point2 - Point1;
	const float SpringLength = SpringDir.Size();

	UE_LOG(LogTemp, Warning, TEXT("Your distance is %s"), *FString::SanitizeFloat(SpringLength));
	return -10.f * (SpringLength - 100);
}