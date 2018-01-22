#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SimpleSpringSystem.generated.h"

// mass spring system in 1 dimension
UCLASS()
class USimpleSpringSystem : public UObject
{
	GENERATED_BODY()
	
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpringSystem")
	float SpringConstant = 40.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpringSystem")
	float SpringDampening = 5.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpringSystem")
	float Mass = 75.0f;

	FORCEINLINE float GetSpringCompression() const { return SpringCompression; }

private:

	float SpringVelcoity = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "SpringSystem", meta = (AllowPrivateAccess = "true"))
	float SpringCompression = 0.0f;

public:

	UFUNCTION(BlueprintCallable, Category = "LegSpring")
	void Tick(float DeltaSeconds);

	UFUNCTION(BlueprintCallable, Category = "LegSpring")
	void AddVelocity(float VelocityChange);
	
};
