#include "ParkourSpectator.h"

#include "SpectatorCameraActor.h"
#include "../ParkourGameCharacter.h"
#include "../MiniGame/MiniGameManager.h"
#include "../UI/ParkourGameHUD.h"

#include "EngineUtils.h"
#include "GameFramework/Character.h"
#include "Camera/PlayerCameraManager.h"
#include "NetworkReplayStreaming.h"
#include "ReplayManager.h"
#include "TimerManager.h"

FParkourSpectatorBroadcasts::FStartActionReplayEvent FParkourSpectatorBroadcasts::StartActionReplay;

static void cmd_OpenMenu(UWorld* World)
{
  APlayerController* PlayerCtlr = World->GetFirstPlayerController();

  // If we have become a pawn using the console command then we use GetPawn, otherwise we need to use spectator pawn
  AParkourSpectator* PlayerCharacter = PlayerCtlr ? Cast<AParkourSpectator>(PlayerCtlr->GetSpectatorPawn()) : nullptr;

#if WITH_EDITOR
  if(!PlayerCharacter)
    PlayerCharacter = Cast<AParkourSpectator>(PlayerCtlr->GetPawn());
#endif

  if (!IsValid(PlayerCharacter))
    return;

  PlayerCharacter->OpenControls();
}

FAutoConsoleCommandWithWorld OpenMenuCmd(
  TEXT("Parkour.OpenControls"),
  TEXT("Open the game controls"),
  FConsoleCommandWithWorldDelegate::CreateStatic(cmd_OpenMenu));


AParkourSpectator::AParkourSpectator()
{
  bReplicates = true;

#if !WITH_EDITOR
  // disable screen messages by default in all non-editor builds
  GAreScreenMessagesEnabled = false;
#endif
}

void AParkourSpectator::BeginPlay()
{
	Super::BeginPlay();

	// gather all availible spectator cameras
	UWorld* WorldPtr = GetWorld();

	if (!WorldPtr)
		return;

	for (TActorIterator<ASpectatorCameraActor> It(WorldPtr); It; ++It)
	{
		AllCameras.Add(*It);
	}

  FParkourSpectatorBroadcasts::StartActionReplay.AddUObject(this, &AParkourSpectator::StartPlayingReplay);

  AReplayManager* ReplayMgr = SingletonHelper.GetSingletonObject<AReplayManager>(WorldPtr);

  if (ReplayMgr)
  {
    ReplayMgr->OnReplayStarted.AddDynamic(this, &AParkourSpectator::TargetNewPlayer);
    ReplayMgr->OnReplayEnded.AddDynamic(this, &AParkourSpectator::TargetNewPlayer);
  }

  AMiniGameManager* MinigameMgr = SingletonHelper.GetSingletonObject<AMiniGameManager>(WorldPtr);

  if (MinigameMgr)
  {
    MinigameMgr->OnTeamScoreUpdated.AddDynamic(this, &AParkourSpectator::TeamScoreUpdated);
  }
}

void AParkourSpectator::SetViewedActor(AActor* NewActor)
{
  GetWorld()->GetTimerManager().ClearTimer(TargetChangeHandle);
  GetWorld()->GetTimerManager().SetTimer(TargetChangeHandle, FTimerDelegate::CreateUObject(this, &AParkourSpectator::TargetNewPlayer), TargetChangeTime, false);

	if (NewActor == ViewedActor) return;

	ViewedActor = NewActor;
	SwitchCamera();
}

void AParkourSpectator::TargetNewPlayer()
{
  if (!IsAutoCam) return;

	AActor* NewTarget = nullptr;

  AReplayManager* Mgr = SingletonHelper.GetSingletonObject<AReplayManager>(GetWorld());

  if (Mgr && Mgr->IsReplaying())
  {
    if (AActor* Targ = ReplayTarget.Get())
    {
      NewTarget = Mgr->GetReplayActorForRealActor(Targ);
    }
    else
    {
      // pick any
      TArray<AParkourGameCharacter*> ValidTargets;
      Mgr->GetAllReplayPlayers(ValidTargets);

      if (ValidTargets.IsValidIndex(0)) NewTarget = ValidTargets[0];
    }
  }
  else
  {
    float CurrentTargetSpeed = -1.0f;

    for (auto it = GetWorld()->GetPawnIterator(); it; ++it)
    {
      if (AParkourGameCharacter* Target = Cast<AParkourGameCharacter>(*it))
      {
        if (Target->HasBall())
        {
          NewTarget = Target;
          break;
        }
        
        const float TargetSpeed = Target->GetVelocity().SizeSquared();

        if (TargetSpeed > CurrentTargetSpeed)
        {
          NewTarget = Target;
          CurrentTargetSpeed = TargetSpeed;
        }
      }
    }
  }

  if(NewTarget)
	  SetViewedActor(NewTarget);
}

void AParkourSpectator::BeginAutoCam()
{
  IsAutoCam = true;
  TargetNewPlayer();
}

void AParkourSpectator::StartGame(TSubclassOf<AMiniGameBase> GameClass)
{
  if (AParkourPlayerController* ControllerPtr = Cast<AParkourPlayerController>(GetController()))
    ControllerPtr->Server_StartGame(GameClass);
}

void AParkourSpectator::EndCurrentGame()
{
  if (AParkourPlayerController* ControllerPtr = Cast<AParkourPlayerController>(GetController()))
    ControllerPtr->Server_EndCurrentGame();
}

void AParkourSpectator::StartActionReplay()
{
  if (AParkourPlayerController* ControllerPtr = Cast<AParkourPlayerController>(GetController()))
    ControllerPtr->Net_StartReplay();
}

void AParkourSpectator::OpenControls()
{
  APlayerController* ControllerPtr = Cast<APlayerController>(GetController());
  AParkourGameHUD* HUDptr = Cast<AParkourGameHUD>(ControllerPtr ? ControllerPtr->GetHUD() : nullptr);
  if (HUDptr)
    HUDptr->OpenSpectatorUI(this);

  InitialiseActionReplay();
}

void AParkourSpectator::TeamScoreUpdated(AMiniGameBase* Game, int32 TeamID)
{
  if (!Game || Game->GetScore(TeamID) == 0 || Role != ENetRole::ROLE_AutonomousProxy) return;

  // wait a small time before starting the replay so that the score moment is in the middle of the playback
  FTimerHandle Handle;
  GetWorld()->GetTimerManager().SetTimer(Handle, FTimerDelegate::CreateUObject(this, &AParkourSpectator::StartPlayingReplay, GetWorld()), 4.0f, false);

  ReplayTarget = Game->GetLastScoringPlayer(TeamID);
}

void AParkourSpectator::InitialiseActionReplay()
{
  UWorld* WorldPtr = GetWorld();

  if (!WorldPtr) return;

  AReplayManager* Mgr = SingletonHelper.Static_GetSingletonObject<AReplayManager>(GetWorld());

  if (Mgr)
  {
    Mgr->SetRecording(true);
  }
}

void AParkourSpectator::StartPlayingReplay(UWorld* World)
{
#if WITH_EDITOR
  if (World != GetWorld()) return;
#endif
  AReplayManager* Mgr = SingletonHelper.GetSingletonObject<AReplayManager>(GetWorld());

  if (!Mgr) return;

  Mgr->StartReplay(8.0f, 0.5f);
}

void AParkourSpectator::SwitchCamera()
{
  GetWorld()->GetTimerManager().ClearTimer(CameraChangeHandle);
  GetWorld()->GetTimerManager().SetTimer(CameraChangeHandle, FTimerDelegate::CreateUObject(this, &AParkourSpectator::SwitchCamera), CameraChangeTime, false);

	ASpectatorCameraActor* Cam = GetBestCamera();

  bool usePlayerView = FMath::RandRange(0, 5) == 0;

  if (!Cam)
  {
    usePlayerView = true;
  }

  if (CurrentCamera)
  {
    CurrentCamera->SetTarget(nullptr);

    if (TempCamera)
    {
      CurrentCamera->Destroy();
    }
  }
  
  TempCamera = false;

  if (usePlayerView && ViewedActor)
  {
    Cam = GetWorld()->SpawnActor<ASpectatorCameraActor>(SpectatorCameraClass.Get());

    if (Cam)
    {
      TempCamera = true;
      Cam->AttachToActor(ViewedActor, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
      Cam->SetActorRelativeLocation(FVector(400.0f, 0.0f, 0.0f));
    }
  }

	CurrentCamera = Cam;

  if (!Cam) return;

	FViewTargetTransitionParams TransitionParms;
	TransitionParms.BlendTime = 0.0f;

	if(APlayerController* ControllerPtr = Cast<APlayerController>(GetController()))
    ControllerPtr->SetViewTarget(Cam, TransitionParms);
  
  Cam->OwningSpectator = this;
  Cam->SetTarget(ViewedActor);
}

ASpectatorCameraActor* AParkourSpectator::GetBestCamera() const
{
	if (AllCameras.Num() < 1 || !ViewedActor) return nullptr;

	ASpectatorCameraActor* Cam = nullptr;
	float SqrdDist = FLT_MAX;

	FVector ViewedActorLoc = ViewedActor->GetActorLocation();
  
  TArray<ASpectatorCameraActor*> ValidCameras;
  for (ASpectatorCameraActor* QryCam : AllCameras)
  {
    //TODO: Do a visibility check with the actor
    const float Dist = (QryCam->GetActorLocation() - ViewedActorLoc).Size();
    if (Dist < QryCam->SwitchCameraDistance)
    {
      ValidCameras.Add(QryCam);
    }
  }

	for (ASpectatorCameraActor* QryCam : ValidCameras)
	{
		float Dist = (QryCam->GetActorLocation() - ViewedActorLoc).SizeSquared();
		if (!Cam || Dist < SqrdDist)
		{
			Cam = QryCam;
			SqrdDist = Dist;
		}
	}

	return Cam;
}

void AParkourSpectator::PossessedBy(class AController* NewController)
{
	Super::PossessedBy(NewController);
	SwitchCamera();
}
