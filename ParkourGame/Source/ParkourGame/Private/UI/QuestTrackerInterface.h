#pragma once

#include "CoreMinimal.h"
#include "Interface.h"
#include "QuestTrackerInterface.generated.h"

UINTERFACE(MinimalAPI)
class UQuestTrackerInterface : public UInterface
{
	GENERATED_BODY()
};

class IQuestTrackerInterface
{
	GENERATED_BODY()

public:
  // Add a new task to the UI
  UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "HUD|Quest", meta = (AutoCreateRefTerm="TaskID,TaskDesc"))
  void AddQuestTask(const FName& TaskID, const FText& TaskDesc);

  // Mark a quest task as completed and remove it from the HUD
  UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "HUD|Quest", meta = (AutoCreateRefTerm = "TaskID"))
  void CompleteQuestTask(const FName& TaskID);

  // Remove a quest task from the HUD without completing it
  UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "HUD|Quest", meta = (AutoCreateRefTerm = "TaskID"))
  void CancelQuestTask(const FName& TaskID);
	
	
};
