#include "ParkourSpectator.h"

#include "SpectatorCameraActor.h"
#include "../ParkourGameCharacter.h"

#include "EngineUtils.h"
#include "GameFramework/Character.h"
#include "Camera/PlayerCameraManager.h"

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

	WorldPtr->GetTimerManager().SetTimer(TargetChangeHandle, FTimerDelegate::CreateUObject(this, &AParkourSpectator::TargetRandomPlayer), TargetChangeTime, true);
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
