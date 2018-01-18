#include "MiniGameBase.h"

#include "MiniGameManager.h"
#include "../Utils/ParkourGameLogging.h"

#include "UnrealNetwork.h"

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

	SelectedTeam->PlayersInTeam.Add(Player);

	//check if we are full
	if (AutoStartWhenTeamsFull &&
		GetNumPlayers() == GetMaxNumPlayers())
	{
		OnGameStart();
	}

	return true;
}

void AMiniGameBase::InitialiseGame(AMiniGameManager* Manager)
{
	if (!ensure(Manager))
	{
		Destroy();
		return;
	}

	GameManager = Manager;
	GameState = EMiniGameSate::Pending;

	TeamData.Empty(NumTeams);

	for (int32 i = 0; i < NumTeams; ++i)
	{
		FMiniGameTeam NewTeam;
		NewTeam.TeamID = i;
		TeamData.Add(NewTeam);
	}
}

void AMiniGameBase::OnGameStart()
{
	// add 5 seconds to allow for network propagation
	CountdownStartTime = FDateTime::Now() + FTimespan(0, 0, 2);
	CountdownStartTime_Replicated = CountdownStartTime.GetTicks();

	GameState = EMiniGameSate::InProgress;

	BPE_GameStart();
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
	for (const FMiniGameTeam& Team : TeamData)
		if (Team.PlayersInTeam.Num() == 0) return false;

	return true;
}

void AMiniGameBase::OnGameEnd(EMiniGameEndReason Reason)
{
	GameState = EMiniGameSate::Complete;
	GameEndReason = Reason;

	FTimerHandle DestroyHandle;
	GetWorld()->GetTimerManager().SetTimer(DestroyHandle, this, &AMiniGameBase::Server_Destroy, 10.0f);

	BPE_GameEnd();
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

		for (const TWeakObjectPtr<AParkourGameCharacter>& PlayerWeakPtr : Team.PlayersInTeam)
		{
			if (AParkourGameCharacter* Player = PlayerWeakPtr.Get())
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

	for (const TWeakObjectPtr<AParkourGameCharacter>& PlayerWeakPtr : FoundTeam->PlayersInTeam)
	{
		if (AParkourGameCharacter* Player = PlayerWeakPtr.Get())
			Players.Add(Player);
	}
}

int32 AMiniGameBase::GetTeamFromPlayer(AParkourGameCharacter* Player) const
{
	for (const FMiniGameTeam& Team : TeamData)
	{
		for (const TWeakObjectPtr<AParkourGameCharacter>& WeakPlayer : Team.PlayersInTeam)
		{
			if (WeakPlayer == Player) return Team.TeamID;
		}
	}

	UE_LOG(ParkourGame, Warning, TEXT("[MinigameBase] Attempted to get the team from a player that is not in this game"));
	return 0;
}

void AMiniGameBase::ModifyScore(int32 TeamID, int32 Change, int32& NewScore)
{
	FMiniGameTeam* TeamPtr = TeamData.FindByPredicate([&](const FMiniGameTeam& queryTeam) {
		return queryTeam.TeamID == TeamID;
	});

	if (!TeamPtr) return;

	TeamPtr->Score += Change;
	TeamPtr->Score = FMath::Max(0, TeamPtr->Score);
	NewScore = TeamPtr->Score;

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

void AMiniGameBase::OnRep_State()
{
	if (!GameManager) return;

	switch (GameState)
	{
	case EMiniGameSate::InProgress:
		GameManager->OnGameStarted.Broadcast(this);
		break;
	case EMiniGameSate::Complete:
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

		if(!oldTeam) continue;

		//compare values are fire the appropriate events
		if (oldTeam->Score != Team.Score) GameManager->OnTeamScoreUpdated.Broadcast(this, Team.TeamID);

		//compare the two lists of players
		TArray<TWeakObjectPtr<AParkourGameCharacter>> mutable_PlayersInTeam = Team.PlayersInTeam;

		for (const TWeakObjectPtr<AParkourGameCharacter>& player : oldTeam->PlayersInTeam)
		{
			int32 foundIdx;
			if (mutable_PlayersInTeam.FindLast(player, foundIdx))
			{
				// The player was in both arrays - nothing has changed
				mutable_PlayersInTeam.RemoveAt(foundIdx);
			}
			else
			{
				// player only in old array - they have left the game
				GameManager->OnPlayerLeftTeam.Broadcast(this, player.Get(), Team.TeamID);
			}
		}

		for (const TWeakObjectPtr<AParkourGameCharacter>& player : mutable_PlayersInTeam)
		{
			// player only in new array - they have joined the game
			GameManager->OnPlayerJoinedTeam.Broadcast(this, player.Get(), Team.TeamID);
		}
	}

	Old_TeamData = TeamData;
}

void AMiniGameBase::OnRep_CountdownTime()
{
	CountdownStartTime = FDateTime(CountdownStartTime_Replicated);
}