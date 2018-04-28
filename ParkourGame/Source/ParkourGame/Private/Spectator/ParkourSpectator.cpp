#include "ParkourSpectator.h"

#include "SpectatorCameraActor.h"
#include "../ParkourGameCharacter.h"
#include "../Utils/SingletonHelper.h"
#include "../MiniGame/MiniGameManager.h"
#include "../UI/ParkourGameHUD.h"

#include "EngineUtils.h"
#include "GameFramework/Character.h"
#include "Camera/PlayerCameraManager.h"

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
}

void AParkourSpectator::SetViewedActor(const AActor* NewActor)
{
	if (NewActor == ViewedActor) return;

	ViewedActor = NewActor;
	SwitchCamera();
}

void AParkourSpectator::TargetRandomPlayer()
{
	// we have to construct this stupid extra list as the raw pawn list container is not exposed
	TArray<const AParkourGameCharacter*> ValidTargets;

	for (auto it = GetWorld()->GetPawnIterator(); it; ++it)
	{
		if (const AParkourGameCharacter* Target = Cast<AParkourGameCharacter>(*it))
			ValidTargets.Add(Target);
	}

	if (ValidTargets.Num() == 0) return;

	SetViewedActor(ValidTargets[FMath::RandHelper(ValidTargets.Num())]);
}

void AParkourSpectator::BeginAutoCam()
{
  GetWorld()->GetTimerManager().SetTimer(TargetChangeHandle, FTimerDelegate::CreateUObject(this, &AParkourSpectator::TargetRandomPlayer), TargetChangeTime, true);
  TargetRandomPlayer();
}

void AParkourSpectator::StartGame(TSubclassOf<AMiniGameBase> GameClass)
{
  AParkourPlayerController* Controller = Cast<AParkourPlayerController>(GetController());

  if (Controller)
    Controller->StartGame(GameClass);
}

void AParkourSpectator::OpenControls()
{
  APlayerController* Controller = Cast<APlayerController>(GetController());
  AParkourGameHUD* HUDptr = Cast<AParkourGameHUD>(Controller ? Controller->GetHUD() : nullptr);
  if (HUDptr)
    HUDptr->OpenSpectatorUI(this);
}

void AParkourSpectator::SwitchCamera()
{
	ASpectatorCameraActor* Cam = GetBestCamera();

	if (!Cam) return;

	if (CurrentCamera) CurrentCamera->SetTarget(nullptr);

	CurrentCamera = Cam;

	FViewTargetTransitionParams TransitionParms;
	TransitionParms.BlendTime = 0.0f;

	if(APlayerController* Controller = Cast<APlayerController>(GetController()))
		Controller->SetViewTarget(Cam, TransitionParms);

	Cam->SetTarget(ViewedActor);
}

ASpectatorCameraActor* AParkourSpectator::GetBestCamera() const
{
	if (AllCameras.Num() < 1 || !ViewedActor) return nullptr;

	ASpectatorCameraActor* Cam = nullptr;
	float SqrdDist = FLT_MAX;

	FVector ViewedActorLoc = ViewedActor->GetActorLocation();

	for (ASpectatorCameraActor* QryCam : AllCameras)
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
