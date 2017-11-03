// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "ParkourGame.h"

#include "GameVersion.h"

#include "Modules/ModuleManager.h"
#include "NetworkVersion.h"

class FParkourGame
	: public FDefaultGameModuleImpl
{
	virtual void StartupModule() override
	{
		FNetworkVersion::GetLocalNetworkVersionOverride.BindRaw(this, &FParkourGame::GetNetworkVersion);
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