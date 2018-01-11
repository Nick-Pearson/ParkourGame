#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"

#include "../ParkourGameCharacter.h"

#include "MiniGameBase.generated.h"

class AMiniGameManager;

UENUM(BlueprintType)
enum class EMiniGameSate : uint8
{
	// Mini game is initialised and waiting for players to join
	Pending,

	// Game is being played
	InProgress,

	// Game over
	Complete,

	MAX
};


UENUM(BlueprintType)
enum class EMiniGameEndReason : uint8
{
	// Not enough players joined to make the game valid to start
	NotEnoughPlayers,

	// A team has won the game
	GameWon,

	// Some players have dropped out of the game making it invalid
	PlayersLeft,

	MAX			UMETA(Hidden)
};

USTRUCT(BlueprintType)
struct FMiniGameTeamUIInfo : public FTableRowBase
{
	GENERATED_BODY()

public:

	// Display name for UI, e.g. display name of A would show UI like "Team A just captured the flag"
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FMiniGameTeamUIInfo")
	FText DisplayName;

	// Colour used for the the team and flags
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FMiniGameTeamUIInfo")
	FLinearColor TeamColour;

};

USTRUCT(BlueprintType)
struct FMiniGameTeam
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly, Category = "MinigameTeam")
	int32 TeamID;

	UPROPERTY(Transient)
	TArray<TWeakObjectPtr<AParkourGameCharacter>> PlayersInTeam;

	UPROPERTY(BlueprintReadOnly, Category = "MinigameTeam")
	int32 Score;
};

UCLASS(abstract)
class AMiniGameBase : public AActor
{
	GENERATED_BODY()

	friend class AMiniGameManager;

public:	
	AMiniGameBase();
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:

	// Called when a player requests to join this game, returns true if the player was able to join
	virtual bool PlayerJoinGame(AParkourGameCharacter* Player);

	// Called when the game object is spawned by the manager
	virtual void InitialiseGame(AMiniGameManager* Manager);

	UFUNCTION(BlueprintCallable, Category = "MiniGame")
	void RequestEndGame(EMiniGameEndReason Reason);

	UFUNCTION(BlueprintCallable, Category = "MiniGame")
	void CheckWinCondition();

	// Blueprint events

	//returns true if the game has been won
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "MiniGame")
	bool IsGameWon() const;

	// returns the team id that is currently winning
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "MiniGame")
	int32 GetWinningTeam();

	// Called when the game is kicked off and the countdown is about to begin
	UFUNCTION(BlueprintImplementableEvent, Category = "MiniGame", meta = (DisplayName="OnGameStart"))
	void BPE_GameStart();
	
	// Called when the game is ending
	UFUNCTION(BlueprintImplementableEvent, Category = "MiniGame", meta = (DisplayName = "OnGameEnd"))
	void BPE_GameEnd();

protected:

	// Called when the game is kicked off
	virtual void OnGameStart();

	// Called when the game is ending
	virtual void OnGameEnd(EMiniGameEndReason Reason);

public:

	// Are the minimum conditions met for a game to be started
	virtual bool CanBeStarted() const;

	UFUNCTION(BlueprintPure, Category = "MiniGame")
	int32 GetNumPlayers() const;

	UFUNCTION(BlueprintPure, Category = "MiniGame")
	int32 GetMaxNumPlayers() const;

	UFUNCTION(BlueprintCallable, Category = "MiniGame")
	void GetAllPlayers(TArray<AParkourGameCharacter*>& AllPlayers) const;

	UFUNCTION(BlueprintCallable, Category = "MiniGame")
	void GetAllPlayersInTeam(int32 TeamID, TArray<AParkourGameCharacter*>& Players) const;

	UFUNCTION(BlueprintCallable, Category = "MiniGame")
	int32 GetTeamFromPlayer(AParkourGameCharacter* Player) const;

	UFUNCTION(BlueprintCallable, Category = "MiniGame")
	void ModifyScore(int32 TeamID, int32 Change, int32& NewScore);

	UFUNCTION(BlueprintCallable, Category = "MiniGame")
	int32 GetScore(int32 TeamID) const;

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MiniGame")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MiniGame")
	FText GameDescription;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MiniGame")
	int32 NumTeams = 2;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MiniGame")
	int32 NumPlayersPerTeam = 5;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MiniGame")
	bool AutoStartWhenTeamsFull = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MiniGame")
	bool ShowCountdown = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MiniGame")
	int32 ScoreToWin = 5;

	UPROPERTY(BlueprintReadOnly, Category = "MiniGame")
	FDateTime CountdownStartTime;

	UFUNCTION()
	void OnRep_State();

	UPROPERTY(BlueprintReadOnly, Category = "MiniGame", ReplicatedUsing = OnRep_State)
	EMiniGameSate GameState = EMiniGameSate::Pending;

	UPROPERTY(BlueprintReadOnly, Category = "MiniGame", Replicated)
	EMiniGameEndReason GameEndReason = EMiniGameEndReason::MAX; // set as variable so it can be read by clients

private:

	void Server_Destroy() { Destroy(); }

	UPROPERTY(Transient, Replicated)
	AMiniGameManager* GameManager;

	UFUNCTION()
	void OnRep_TeamData();

	UPROPERTY(Transient, ReplicatedUsing = OnRep_TeamData)
	TArray<FMiniGameTeam> TeamData;

	UFUNCTION()
	void OnRep_CountdownTime();

	UPROPERTY(Transient, ReplicatedUsing = OnRep_CountdownTime)
	int64 CountdownStartTime_Replicated;

	// Required for networking - using the parameter version of ReplicatedUsing seems to be broken when using a TArray nested in a struct
	TArray<FMiniGameTeam> Old_TeamData;
};
