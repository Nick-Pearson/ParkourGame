#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MiniGameBase.h"
#include "MiniGameManager.generated.h"

class AParkourGameCharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMinigameEvent, AMiniGameBase*, Game);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FMinigameEndedEvent, AMiniGameBase*, Game, EMiniGameEndReason, Reason);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FMinigameTeamEvent, AMiniGameBase*, Game, int32, TeamID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FMinigamePlayerTeamEvent, AMiniGameBase*, Game, AParkourGameCharacter*, Player, int32, TeamID);

UCLASS()
class AMiniGameManager : public AActor
{
	GENERATED_BODY()
	
public:	
	AMiniGameManager();

	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
public:

  UFUNCTION(BlueprintCallable, Category = "MinigameManager")
	void CreateRandomGame();

  UFUNCTION(BlueprintCallable, Category = "MinigameManager")
	void CreateGame(TSubclassOf<AMiniGameBase> GameClass);

	bool AddPlayerToGame(AParkourGameCharacter* Player);

	UFUNCTION(BlueprintCallable, Category = "MinigameManager")
	void RemovePlayerFromGame(AParkourGameCharacter* Player);
	
	UFUNCTION(BlueprintCallable, Category = "MinigameManager")
	void GetUIDataForTeam(int32 index, FMiniGameTeamUIInfo& outInfo) const;

	FORCEINLINE AMiniGameBase* GetActiveGame() const { return ActiveGame; }

	void EndGame(AMiniGameBase* Game, EMiniGameEndReason Reason);

public:
	// EVENTS
	UPROPERTY(BlueprintAssignable, Category = "MinigameManager|Events")
	FMinigameEvent OnGameStarted;

	UPROPERTY(BlueprintAssignable, Category = "MinigameManager|Events")
	FMinigameEndedEvent OnGameOver;

	UPROPERTY(BlueprintAssignable, Category = "MinigameManager|Events")
	FMinigameTeamEvent OnTeamScoreUpdated;

	UPROPERTY(BlueprintAssignable, Category = "MinigameManager|Events")
	FMinigamePlayerTeamEvent OnPlayerJoinedTeam;

	UPROPERTY(BlueprintAssignable, Category = "MinigameManager|Events")
	FMinigamePlayerTeamEvent OnPlayerLeftTeam;

public:

	UPROPERTY(EditAnywhere, Category = "MinigameManager")
	TArray<TSubclassOf<AMiniGameBase>> AvailibleMiniGames;

	UPROPERTY(EditAnywhere, Category = "MinigameManager", meta = (RowType ="MiniGameTeamUIInfo"))
	class UDataTable* TeamUIInfoTable;

	// Whether the map uses minigames or a single game mode for the map
	UPROPERTY(EditAnywhere, Category = "MiniGameManager")
	bool SingleGame = false;

	// If the game is not full by the end of this timeout it is either destroyed or automatically started
	UPROPERTY(EditAnywhere, Category = "MiniGameManager")
	float GameStartTimeout = 30;

	// how long between when random games are started
	UPROPERTY(EditAnywhere, Category = "MiniGameManager")
	float GameCreationInterval = 300;
private:

	// check on a pending game - if it is still not ready to be started then we cancel it
	void CheckUpOnGame();

	void SetNextGameTimer();

	UFUNCTION()
	void OnGameEnded(AMiniGameBase* Game, EMiniGameEndReason Reason);

	UPROPERTY(BlueprintReadOnly, Category = "MinigameManager", Transient, Replicated, meta = (AllowPrivateAccess = "true"))
	AMiniGameBase* ActiveGame;

	FTimerHandle NextGameTimer;
	FTimerHandle CheckGameTimer;
};
