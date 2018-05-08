#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameVersion.generated.h"

#define GAME_VERSION_MAJOR 0
#define GAME_VERSION_MINOR 11
#define GAME_VERSION_PATCH 2

USTRUCT(BlueprintType)
struct FGameVersion
{
public:

	GENERATED_BODY()

	FGameVersion(uint32 inMajor = 1, uint32 inMinor = 0, uint32 inPatch = 0) :
		Major(inMajor), Minor(inMinor), Patch(inPatch)
	{}

	uint32 Major, Minor, Patch;

public:

	static FGameVersion Current()
	{
		return FGameVersion(GAME_VERSION_MAJOR, GAME_VERSION_MINOR, GAME_VERSION_PATCH);
	}

	bool operator<(const FGameVersion& other) const
	{
		return GetComparisonVal() < other.GetComparisonVal();
	}

	bool operator==(const FGameVersion& other) const
	{
		return Major == other.Major && Minor == other.Minor && Patch == other.Patch;
	}

	uint32 GetComparisonVal() const
	{
		return (Major * 10000) + (Minor * 100) + Patch;
	}
};

UCLASS()
class UGameVersionBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintPure, Category = "GameVersion")
	static FGameVersion GetCurrentVersion();

	UFUNCTION(BlueprintPure, Category = "Math|GameVersion", meta = (DisplayName = "Equal (GameVersion)", CompactNodeTitle = "=="))
	static bool Equals_GameVersion(const FGameVersion& A, const FGameVersion& B);

	UFUNCTION(BlueprintPure, Category = "Math|GameVersion", meta = (DisplayName = "GameVersion < GameVersion", CompactNodeTitle = "<"))
	static bool LT_GameVersion(const FGameVersion& A, const FGameVersion& B);
	
};