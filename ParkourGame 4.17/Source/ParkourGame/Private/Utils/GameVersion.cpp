#include "GameVersion.h"

FGameVersion UGameVersionBlueprintLibrary::GetCurrentVersion()
{
	return FGameVersion::Current();
}

bool UGameVersionBlueprintLibrary::Equals_GameVersion(const FGameVersion& A, const FGameVersion& B)
{
	return A == B;
}

bool UGameVersionBlueprintLibrary::LT_GameVersion(const FGameVersion& A, const FGameVersion& B)
{
	return A < B;
}
