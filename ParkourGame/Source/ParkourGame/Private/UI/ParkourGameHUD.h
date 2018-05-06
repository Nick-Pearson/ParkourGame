#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"

#include "ToasterInterface.h"
#include "QuestTrackerInterface.h"

#include "ParkourGameHUD.generated.h"

UCLASS()
class AParkourGameHUD : public AHUD
{
	GENERATED_BODY()

public:

	FORCEINLINE UObject* GetToaster() const { return Toaster.GetObject(); }
  FORCEINLINE UObject* GetQuestTracker() const { return QuestTracker.GetObject(); }

  UFUNCTION(BlueprintImplementableEvent)
  void OpenSpectatorUI(AParkourSpectator* Spectator);

  UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
  void OpenLeadboardUI(const TArray<FString>& TeamAScores, const TArray<FString>& TeamBScores);

  UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
  void CloseLeaderboardUI();

  UFUNCTION(BlueprintImplementableEvent)
  void InitialisePlayerUI();

private:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD", meta = (AllowPrivateAccess = "true"))
	TScriptInterface<IToasterInterface> Toaster;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD", meta = (AllowPrivateAccess = "true"))
  TScriptInterface<IQuestTrackerInterface> QuestTracker;
		
};
