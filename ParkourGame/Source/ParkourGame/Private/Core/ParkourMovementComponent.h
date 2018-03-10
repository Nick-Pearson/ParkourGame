#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ParkourMovementComponent.generated.h"

UCLASS()
class UParkourMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
	
public:

	UPROPERTY(BlueprintReadWrite, Category = "Parkour")
	bool bEnableGravity = true;

	virtual float GetGravityZ() const override;

  UFUNCTION(BlueprintCallable, Category = "Parkour")
  void SetMaxAcceleration(float NewMaxAcceleration);

};
