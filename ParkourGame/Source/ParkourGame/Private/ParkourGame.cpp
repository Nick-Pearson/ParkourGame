// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "ParkourGame.h"

#include "Utils/GameVersion.h"

#include "Modules/ModuleManager.h"
#include "NetworkVersion.h"

static TAutoConsoleVariable<int32> CVarServerNumber(
	TEXT("serverNumber"),
	0,
	TEXT("Defines the server number that this client will connect to.\n")
	TEXT("  0: default\n"),
	ECVF_Scalability | ECVF_RenderThreadSafe);

class FParkourGame
	: public FDefaultGameModuleImpl
{
	virtual void StartupModule() override
	{
		FNetworkVersion::GetLocalNetworkVersionOverride.BindRaw(this, &FParkourGame::GetNetworkVersion);

#if !WITH_EDITOR
    // disable screen messages by default in all non-editor builds
    GAreScreenMessagesEnabled = false;
#endif
	}

	virtual void ShutdownModule() override
	{
		FNetworkVersion::GetLocalNetworkVersionOverride.Unbind();
	}

	uint32 GetNetworkVersion() const
	{
		return FGameVersion::Current().GetComparisonVal();
	}
};

IMPLEMENT_PRIMARY_GAME_MODULE(FParkourGame, ParkourGame, "ParkourGame")