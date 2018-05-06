#include "MiniGameBase.h"

#include "MiniGameManager.h"
#include "../Utils/ParkourGameLogging.h"
#include "../Core/ParkourGameState.h"
#include "LeaderboardFunctionLibrary.h"
#include "../ParkourGameCharacter.h"

#include "UnrealNetwork.h"
#include "TimerManager.h"
#include "GameFramework/PlayerState.h"

AMiniGameBase::AMiniGameBase()
{
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;
	bAlwaysRelevant = true;
}

void AMiniGameBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMiniGameBase, CountdownStartTime_Replicated);
	DOREPLIFETIME(AMiniGameBase, GameState);
	DOREPLIFETIME(AMiniGameBase, GameEndReason);
	DOREPLIFETIME(AMiniGameBase, GameManager);
	DOREPLIFETIME(AMiniGameBase, TeamData);
}

bool AMiniGameBase::PlayerJoinGame(AParkourGameCharacter* Player)
{
	if (!Player || !HasAuthority()) return false;

	// ensure the player isn't already in the game
	TArray<AParkourGameCharacter*> AllPlayers;
	GetAllPlayers(AllPlayers);

	if (AllPlayers.Contains(Player)) return false;

	//get the team with the lowest number of players
	FMiniGameTeam* SelectedTeam = nullptr;
	for (FMiniGameTeam& Team : TeamData)
	{
		if (!SelectedTeam || SelectedTeam->PlayersInTeam.Num() > Team.PlayersInTeam.Num())
			SelectedTeam = &Team;
	}

	if (!ensure(SelectedTeam)) return false;

	if (SelectedTeam->PlayersInTeam.Num() >= NumPlayersPerTeam)
	{
		UE_LOG(ParkourGame, Warning, TEXT("Player unable to join Minigame as the game is full!"));
		return false;
	}

	SelectedTeam->PlayersInTeam.Add(FPlayerInTeam(Player));

	//check if we are full
	if (AutoStartWhenTeamsFull &&
		GetNumPlayers() == GetMaxNumPlayers())
	{
		OnGameStart();
	}

	return true;
}

void AMiniGameBase::PlayerLeaveGame(AParkourGameCharacter* Player)
{
	if (!Player || !HasAuthority()) return;

	// ensure the player is already in the game
  int32 TeamID = GetTeamFromPlayer(Player);
  
	if (TeamID == INDEX_NONE) return;

	FMiniGameTeam Team = GetTeamFromID(TeamID);
  int32 RemoveIdx = Team.PlayersInTeam.FindLastByPredicate([&](const FPlayerInTeam& Plr) {
    return Plr.PlayerPtr == Player;
  });
  if (RemoveIdx != INDEX_NONE) Team.PlayersInTeam.RemoveAtSwap(RemoveIdx);

	//check if Team is empty
	if (Team.PlayersInTeam.Num() == 0)
	{
    RequestEndGame(EMiniGameEndReason::PlayersLeft);
	}

	return;
}

void AMiniGameBase::InitialiseGame(AMiniGameManager* Manager)
{
	if (!ensure(Manager))
	{
		Destroy();
		return;
	}

	

	GameManager = Manager;
	GameState = EMiniGameState::Pending;

	OnGameInit();

	if (NumPlayersPerTeam == 1)
		FreeForAll = true;

	TeamData.Empty(NumTeams);

	for (int32 i = 0; i < NumTeams; ++i)
	{
		FMiniGameTeam NewTeam;
		NewTeam.TeamID = i;
		NewTeam.Score = 0;
		TeamData.Add(NewTeam);
	}
}

void AMiniGameBase::OnGameStart()
{
	// add 2 seconds to allow for network propagation
	CountdownStartTime = FDateTime::Now() + FTimespan(0, 0, 2);
	CountdownStartTime_Replicated = CountdownStartTime.GetTicks();

	GameState = EMiniGameState::InProgress;

  if (UWorld* WorldPtr = GetWorld())
  {
    AParkourGameState * GS = WorldPtr->GetGameState<AParkourGameState>();
    GS->Net_PlayAnnouncerSound(EAnnouncerSound::Prepare);

    FTimerHandle Handle;
    WorldPtr->GetTimerManager().SetTimer(Handle, FTimerDelegate::CreateUObject(GS, &AParkourGameState::Net_PlayAnnouncerSound, EAnnouncerSound::Begin), 4.0f, false);
  }

	BPE_GameStart();
}

void AMiniGameBase::OnGameInit()
{
	BPE_GameInit();
}

void AMiniGameBase::RequestEndGame(EMiniGameEndReason Reason)
{
	if (GameManager) GameManager->EndGame(this, Reason);
	else Destroy();
}

void AMiniGameBase::CheckWinCondition()
{
	if (!IsGameWon()) return;

	RequestEndGame(EMiniGameEndReason::GameWon);
}

bool AMiniGameBase::IsGameWon_Implementation() const
{
	for (const FMiniGameTeam& Team : TeamData)
	{
		if (Team.Score >= ScoreToWin) return true;
	}

	return false;
}

int32 AMiniGameBase::GetWinningTeam_Implementation()
{
	int32 WinningTeam = -1;
	int32 WinningScore = -1;

	for (const FMiniGameTeam& Team : TeamData)
	{
		if (Team.Score > WinningScore)
		{
			WinningTeam = Team.TeamID;
			WinningScore = Team.Score;
		}
	}

	return WinningTeam;
}

bool AMiniGameBase::CanBeStarted() const
{
	// only condition currently is that every team has at least one player
	for (const FMiniGameTeam& Team : TeamData) {
		if (Team.PlayersInTeam.Num() == 0 && !FreeForAll) {
			UE_LOG(ParkourGame, Warning, TEXT("[MinigameBase] team does not have enough players, players in team = %d"), Team.PlayersInTeam.Num()); return false;;
		}
	}
	return true;
}

void AMiniGameBase::OnGameEnd(EMiniGameEndReason Reason)
{
	GameState = EMiniGameState::Complete;
	GameEndReason = Reason;

	FTimerHandle DestroyHandle;
	GetWorld()->GetTimerManager().SetTimer(DestroyHandle, this, &AMiniGameBase::Server_Destroy, 10.0f);

	BPE_GameEnd();

  if (Reason != EMiniGameEndReason::NotEnoughPlayers && HasAuthority())
  {
    TArray<FNewLeaderboardData> Entries;
    Entries.Reserve(GetNumPlayers());

    for (const FMiniGameTeam& Team : TeamData)
    {
      EGameOutcome TeamOutcome = (GetWinningTeam() == Team.TeamID) ? EGameOutcome::Win : EGameOutcome::Loss;

      for (const FPlayerInTeam& Player : Team.PlayersInTeam)
      {
        AParkourGameCharacter* PlayerPtr = Player.PlayerPtr.Get();

        if(!PlayerPtr) continue;
        
        Entries.Add(FNewLeaderboardData(PlayerPtr->PlayerState->PlayerName, Player.Goals, Player.OwnGoals, TeamOutcome));
      }
    }

    FPostLeaderboardSignature Callback;
    ULeaderboardFunctionLibrary::PostLeaderboardEntries(Callback, Entries);
  }
}

int32 AMiniGameBase::GetNumPlayers() const
{
	int32 Sum = 0;
	for (const FMiniGameTeam& Team : TeamData)
		Sum += Team.PlayersInTeam.Num();

	return Sum;
}

int32 AMiniGameBase::GetMaxNumPlayers() const
{
	return NumPlayersPerTeam * NumTeams;
}

void AMiniGameBase::GetAllPlayers(TArray<AParkourGameCharacter*>& AllPlayers) const
{
	AllPlayers.Empty(0);
	for (const FMiniGameTeam& Team : TeamData)
	{
		AllPlayers.Reserve(AllPlayers.Num() + Team.PlayersInTeam.Num());

		for (const FPlayerInTeam& Plr : Team.PlayersInTeam)
		{
      if (AParkourGameCharacter* Player = Plr.PlayerPtr.Get())
				AllPlayers.Add(Player);
		}
	}
}

void AMiniGameBase::GetAllPlayersInTeam(int32 TeamID, TArray<AParkourGameCharacter*>& Players) const
{
	const FMiniGameTeam* FoundTeam = TeamData.FindByPredicate([&](const FMiniGameTeam& QueryTeam) {
		return QueryTeam.TeamID == TeamID;
	});

	if (!FoundTeam) return;

	for (const FPlayerInTeam& Plr : FoundTeam->PlayersInTeam)
	{
		if (AParkourGameCharacter* Player = Plr.PlayerPtr.Get())
			Players.Add(Player);
	}
}

void AMiniGameBase::GetAllPlayerNamesInTeam(int32 TeamID, TArray<FString>& outNames) const
{
  const FMiniGameTeam* FoundTeam = TeamData.FindByPredicate([&](const FMiniGameTeam& QueryTeam) {
    return QueryTeam.TeamID == TeamID;
  });

  if (!FoundTeam) return;

  for (const FPlayerInTeam& Plr : FoundTeam->PlayersInTeam)
  {
    if (AParkourGameCharacter* Player = Plr.PlayerPtr.Get())
      outNames.Add(Player->PlayerState->PlayerName);
  }
}

int32 AMiniGameBase::GetTeamFromPlayer(AParkourGameCharacter* Player) const
{
  if (!Player) return INDEX_NONE;

	for (const FMiniGameTeam& Team : TeamData)
	{
		for (const FPlayerInTeam& Plr : Team.PlayersInTeam)
		{
			if (Plr.PlayerPtr == Player) return Team.TeamID;
		}
	}

	UE_LOG(ParkourGame, Warning, TEXT("[MinigameBase] Attempted to get the team from a player that is not in this game"));
	return INDEX_NONE;
}

FMiniGameTeam AMiniGameBase::GetTeamFromID(int32 TeamID) const
{
	for (const FMiniGameTeam& Team : TeamData)
	{
			if (TeamID == Team.TeamID) return Team;
	}

	UE_LOG(ParkourGame, Warning, TEXT("[MinigameBase] Attempted to get the team from an invalid ID"));

	FMiniGameTeam ErrTeam;
	ErrTeam.TeamID = -1;

	return ErrTeam;
}

void AMiniGameBase::ModifyScore(int32 TeamID, int32 Change, AParkourGameCharacter* Scorer, int32& NewScore)
{
	FMiniGameTeam* TeamPtr = TeamData.FindByPredicate([&](const FMiniGameTeam& queryTeam) {
		return queryTeam.TeamID == TeamID;
	});

	if (!TeamPtr) return;

	TeamPtr->Score += Change;
	TeamPtr->Score = FMath::Max(0, TeamPtr->Score);
	NewScore = TeamPtr->Score;

  int32 ScorerTeamID = GetTeamFromPlayer(Scorer);

  if (ScorerTeamID != INDEX_NONE)
  {
    //handle the own goal case
    if (ScorerTeamID != TeamID)
    {
      FMiniGameTeam* ScorerTeamPtr = TeamData.FindByPredicate([&](const FMiniGameTeam& queryTeam) {
        return queryTeam.TeamID == ScorerTeamID;
      });

      FPlayerInTeam* PlayerData = ScorerTeamPtr->PlayersInTeam.FindByPredicate([&](const FPlayerInTeam& Plr) {
        return Plr.PlayerPtr == Scorer;
      });

      PlayerData->OwnGoals += Change;
    }
    else
    {
      FPlayerInTeam* PlayerData = TeamPtr->PlayersInTeam.FindByPredicate([&](const FPlayerInTeam& Plr) {
        return Plr.PlayerPtr == Scorer;
      });

      PlayerData->Goals += Change;
    }
  }
  
  TeamPtr->LastScoringPlayer = Scorer;

	CheckWinCondition();
}

int32 AMiniGameBase::GetScore(int32 TeamID) const
{
	const FMiniGameTeam* TeamPtr = TeamData.FindByPredicate([&](const FMiniGameTeam& queryTeam) {
		return queryTeam.TeamID == TeamID;
	});

	if (!TeamPtr) return 0;

	return TeamPtr->Score;
}

AParkourGameCharacter* AMiniGameBase::GetLastScoringPlayer(int32 TeamID) const
{
  const FMiniGameTeam* ScorerTeamPtr = TeamData.FindByPredicate([&](const FMiniGameTeam& queryTeam) {
    return queryTeam.TeamID == TeamID;
  });

  if (!ScorerTeamPtr) return nullptr;
  return ScorerTeamPtr->LastScoringPlayer.Get();
}

void AMiniGameBase::OnRep_State()
{
	if (!GameManager) return;

	switch (GameState)
	{
	case EMiniGameState::InProgress:
		GameManager->OnGameStarted.Broadcast(this);
		break;
	case EMiniGameState::Complete:
		GameManager->OnGameOver.Broadcast(this, GameEndReason);
		break;
	}
}

void AMiniGameBase::OnRep_TeamData()
{
	if (!GameManager) return;

	for (const FMiniGameTeam& Team : TeamData)
	{
		//get the old team data
		const FMiniGameTeam* oldTeam = Old_TeamData.FindByPredicate([&](const FMiniGameTeam& queryTeam) {
			return queryTeam.TeamID == Team.TeamID;
		});

		if (!oldTeam)
		{
			// no existing team data so all players are new players
			GameManager->OnTeamScoreUpdated.Broadcast(this, Team.TeamID);

			for (const FPlayerInTeam& player : Team.PlayersInTeam)
			{
				// player only in new array - they have joined the game
				GameManager->OnPlayerJoinedTeam.Broadcast(this, player.PlayerPtr.Get(), Team.TeamID);
			}

			continue;
		}

		//compare values are fire the appropriate events
		if (oldTeam->Score != Team.Score) GameManager->OnTeamScoreUpdated.Broadcast(this, Team.TeamID);

		//compare the two lists of players
		TArray<FPlayerInTeam> mutable_PlayersInTeam = Team.PlayersInTeam;

		for (const FPlayerInTeam& player : oldTeam->PlayersInTeam)
		{
      int32 foundIdx = mutable_PlayersInTeam.FindLastByPredicate([&](const FPlayerInTeam& plr) {
        return plr.PlayerPtr == player.PlayerPtr;
      });

			if (foundIdx != INDEX_NONE)
			{
				// The player was in both arrays - nothing has changed
				mutable_PlayersInTeam.RemoveAt(foundIdx);
			}
			else
			{
				// player only in old array - they have left the game
				GameManager->OnPlayerLeftTeam.Broadcast(this, player.PlayerPtr.Get(), Team.TeamID);
			}
		}

		for (const FPlayerInTeam& player : mutable_PlayersInTeam)
		{
			// player only in new array - they have joined the game
			GameManager->OnPlayerJoinedTeam.Broadcast(this, player.PlayerPtr.Get(), Team.TeamID);
		}
	}

	Old_TeamData = TeamData;
}

void AMiniGameBase::OnRep_CountdownTime()
{
	CountdownStartTime = FDateTime(CountdownStartTime_Replicated);
}

void AMiniGameBase::SetMiniGameConfig(FText Name, FText Description, int32 NTeams, int32 PlayersPerTeam, int32 PointsToWin, bool JoinAutomatically, int32 GameLengthMax)
{
	DisplayName = Name;
	GameDescription = Description;
	NumTeams = NTeams;
	NumPlayersPerTeam = PlayersPerTeam;
	ScoreToWin = PointsToWin;
	AutoJoin = JoinAutomatically;
	GameMaxLength = GameLengthMax;
}