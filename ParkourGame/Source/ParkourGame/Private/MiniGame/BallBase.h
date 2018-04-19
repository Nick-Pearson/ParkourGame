#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BallBase.generated.h"

class AParkourGameCharacter;

UCLASS(abstract)
class ABallBase : public AActor
{
	GENERATED_BODY()
	
public:	
	ABallBase();
	
  void SetHeldBy(AParkourGameCharacter* Character);

  FORCEINLINE AParkourGameCharacter* GetHeldBy() const { return HeldBy.Get(); }

private:

  TWeakObjectPtr<AParkourGameCharacter> HeldBy;
};
