#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PushSpringSystem.generated.h"

UCLASS()
class UPushSpringSystem : public UObject
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpringSystem", meta = (AllowPrivateAccess = "true"))
	FVector Point2;
private:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SpringSystem", meta = (AllowPrivateAccess = "true"))
	FVector Point1;

public:

	UFUNCTION(BlueprintCallable, Category = "SpringSystem")
	void Initialise(const FVector& inPoint1, const FVector& inPoint2);

	UFUNCTION(BlueprintPure, Category = "SpringSystem")
	FVector GetSpringForce(const FVector& newPoint2) const;

	FORCEINLINE FVector GetPoint1Location() const { return Point1; }

};