#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"

#include "../ParkourGameCharacter.h"

#include "MiniGameBase.generated.h"

class AMiniGameManager;

UENUM(BlueprintType)
enum class EMiniGameState : uint8
{
	// Mini game is initialised and waiting for players to join
	Pending,

	// Game is being played
	InProgress,

	// Game over
	Complete,

	MAX
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

  // Material used on players
  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FMiniGameTeamUIInfo")
  UMaterialInterface* PlayerMaterial;

  // Material used on players
  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FMiniGameTeamUIInfo")
  UMaterialInterface* HatMaterial;

};

USTRUCT()
struct FPlayerInTeam
{
  GENERATED_BODY()
public:

  FPlayerInTeam(AParkourGameCharacter* Player = nullptr) : 
    PlayerPtr(Player), Goals(0), OwnGoals(0)
  {}

  UPROPERTY(Transient)
  TWeakObjectPtr<AParkourGameCharacter> PlayerPtr;

  UPROPERTY(Transient)
  int32 Goals = 0;

  UPROPERTY(Transient)
  int32 OwnGoals = 0;

};

USTRUCT(BlueprintType)
struct FMiniGameTeam
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly, Category = "MinigameTeam")
	int32 TeamID;

	UPROPERTY(Transient)
	TArray<FPlayerInTeam> PlayersInTeam;

  UPROPERTY(Transient)
  TWeakObjectPtr<AParkourGameCharacter> LastScoringPlayer;

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

	// Called when a player leaves the game
	virtual void PlayerLeaveGame(AParkourGameCharacter* Player);

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

	// Called when the game loaded, used to set game parameters
	UFUNCTION(BlueprintImplementableEvent, Category = "MiniGame", meta = (DisplayName = "OnGameInit"))
	void BPE_GameInit();

	// Called when the game is kicked off and the countdown is about to begin
	UFUNCTION(BlueprintImplementableEvent, Category = "MiniGame", meta = (DisplayName="OnGameStart"))
	void BPE_GameStart();
	
	// Called when the game is ending
	UFUNCTION(BlueprintImplementableEvent, Category = "MiniGame", meta = (DisplayName = "OnGameEnd"))
	void BPE_GameEnd();

protected:

	// Called when game is being loaded
	virtual void OnGameInit();

	// Called when the game is kicked off
	virtual void OnGameStart();

	// Called when the game is ending
	virtual void OnGameEnd(EMiniGameEndReason Reason);

public:

	// Are the minimum conditions met for a game to be started
	virtual bool CanBeStarted() const;

	UFUNCTION(BlueprintCallable, Category = "MiniGame")
	void SetMiniGameConfig(FText Name, FText Description, int32 NTeams, int32 PlayersPerTeam, int32 PointsToWin, bool JoinAutomatically, int32 GameLengthMax);

	UFUNCTION(BlueprintPure, Category = "MiniGame")
	int32 GetNumPlayers() const;

	UFUNCTION(BlueprintPure, Category = "MiniGame")
	int32 GetMaxNumPlayers() const;

	UFUNCTION(BlueprintCallable, Category = "MiniGame")
	void GetAllPlayers(TArray<AParkourGameCharacter*>& AllPlayers) const;

	UFUNCTION(BlueprintCallable, Category = "MiniGame")
	void GetAllPlayersInTeam(int32 TeamID, TArray<AParkourGameCharacter*>& Players) const;

  UFUNCTION(BlueprintCallable, Category = "MiniGame")
  void GetAllPlayerNamesInTeam(int32 TeamID, TArray<FString>& outNames) const;

	UFUNCTION(BlueprintCallable, Category = "MiniGame")
	int32 GetTeamFromPlayer(AParkourGameCharacter* Player) const;

	UFUNCTION(BlueprintCallable, Category = "MiniGame")
	FMiniGameTeam GetTeamFromID(int32 TeamID) const;

	UFUNCTION(BlueprintCallable, Category = "MiniGame")
	void ModifyScore(int32 TeamID, int32 Change, AParkourGameCharacter* Scorer, int32& NewScore);

	UFUNCTION(BlueprintCallable, Category = "MiniGame")
	int32 GetScore(int32 TeamID) const;

  UFUNCTION(BlueprintCallable, Category = "MiniGame")
  AParkourGameCharacter* GetLastScoringPlayer(int32 TeamID) const;

  FORCEINLINE AMiniGameManager* GetManager() const { return GameManager; }

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MiniGame")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MiniGame")
	FText GameDescription;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MiniGame")
	int32 NumTeams = 2;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MiniGame")
	int32 NumPlayersPerTeam = 100;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MiniGame")
	bool AutoStartWhenTeamsFull = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MiniGame")
	bool ShowCountdown = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MiniGame")
	int32 ScoreToWin = 5;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MiniGame")
	int32 GameMaxLength = 300;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MiniGame")
	bool AutoJoin = true;

	UPROPERTY(BlueprintReadOnly, Category = "MiniGame")
	FDateTime CountdownStartTime;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MiniGame")
	bool FreeForAll = false; //to improve readability

	UFUNCTION()
	void OnRep_State();

	UPROPERTY(BlueprintReadOnly, Category = "MiniGame", ReplicatedUsing = OnRep_State)
	EMiniGameState GameState = EMiniGameState::Pending;

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
