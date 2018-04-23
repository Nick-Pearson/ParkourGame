#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"

#include "ToasterInterface.h"

#include "ParkourGameHUD.generated.h"

UCLASS()
class AParkourGameHUD : public AHUD
{
	GENERATED_BODY()

public:

	FORCEINLINE UObject* GetToaster() const { return Toaster.GetObject(); }

  UFUNCTION(BlueprintImplementableEvent)
  void OpenSpectatorUI(AParkourSpectator* Spectator);

private:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD", meta = (AllowPrivateAccess = "true"))
	TScriptInterface<IToasterInterface> Toaster;
		
};
