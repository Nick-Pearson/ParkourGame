#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SpringSystem.generated.h"

// 3D dampened spring system modelling an attachment to a fixed point (point 1)
UCLASS()
class USpringSystem : public UObject
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpringSystem")
	float SpringConstant = 40.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpringSystem")
	float SpringDampening = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpringSystem")
	float RestingSpringExtension = 50.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpringSystem", meta = (AllowPrivateAccess = "true"))
	FVector Point2;
private:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SpringSystem", meta = (AllowPrivateAccess = "true"))
	FVector Point1;

	FVector SpringForce;
	FVector OldPoint2;

public:

	UFUNCTION(BlueprintCallable, Category = "SpringSystem")
	void Initialise(const FVector& inPoint1, const FVector& inPoint2);

	UFUNCTION(BlueprintCallable, Category = "SpringSystem")
	void Tick(float DeltaTime);

	UFUNCTION(BlueprintPure, Category = "SpringSystem")
	FVector GetSpringForce() const;

	FORCEINLINE FVector GetPoint1Location() const { return Point1; }

};
