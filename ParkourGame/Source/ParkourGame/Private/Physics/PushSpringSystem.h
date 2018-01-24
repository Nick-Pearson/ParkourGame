#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PushSpringSystem.generated.h"

// 3D dampened spring system modelling an attachment to a fixed point (point 1)
UCLASS()
class UPushSpringSystem : public UObject
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PushSpringSystem")
	float SpringConstant = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PushSpringSystem")
	float SpringDampening = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PushSpringSystem")
	float RestingSpringExtension = 50.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PushSpringSystem", meta = (AllowPrivateAccess = "true"))
	FVector Point3;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PushSpringSystem", meta = (AllowPrivateAccess = "true"))
	FVector Point2;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PushSpringSystem", meta = (AllowPrivateAccess = "true"))
	FVector Point1;

private:

	

	UFUNCTION(BlueprintCallable, Category = "PushSpringSystem")
	void CalculatePoint2();
	
	AActor* Actor;
	FVector SpringForce;
	FVector OldPoint2;

public:

	UFUNCTION(BlueprintCallable, Category = "PushSpringSystem")
	void Initialise(const FVector& start, const FVector& end, AActor* ActorToIgnore);

	UFUNCTION(BlueprintCallable, Category = "PushSpringSystem")
	void Tick(float DeltaTime);

	UFUNCTION(BlueprintPure, Category = "PushSpringSystem")
	float GetSpringForce() const;

};
