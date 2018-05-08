#include "ParkourHelperLibrary.h"
#include "ParkourFNames.h"
#include "Regex.h"

FName UParkourHelperLibrary::GetRootBoneForBodyPart(EBodyPart Part)
{
	TArray<FName> Chain;
	GetBoneChainForBodyPart(Part, Chain);

	return Chain.IsValidIndex(0) ? Chain[0] : NAME_None;
}

FName UParkourHelperLibrary::GetEndBoneForBodyPart(EBodyPart Part)
{
	TArray<FName> Chain;
	GetBoneChainForBodyPart(Part, Chain);

	return Chain.IsValidIndex(Chain.Num() - 1) ? Chain[Chain.Num() - 1] : NAME_None;
}

void UParkourHelperLibrary::GetBoneChainForBodyPart(EBodyPart Part, TArray<FName>& outChain)
{
	switch (Part)
	{
	case EBodyPart::Head:
		outChain = TArray<FName>({FParkourFNames::Bone_Head});
		break;
	case EBodyPart::Torso:
		outChain = TArray<FName>({FParkourFNames::Bone_Spine_01, FParkourFNames::Bone_Spine_02, FParkourFNames::Bone_Spine_03});
		break;
	case EBodyPart::LeftArm:
		outChain = TArray<FName>({ FParkourFNames::Bone_Upperarm_L, FParkourFNames::Bone_Lowerarm_L, FParkourFNames::Bone_Hand_L });
		break;
	case EBodyPart::RightArm:
		outChain = TArray<FName>({ FParkourFNames::Bone_Upperarm_R, FParkourFNames::Bone_Lowerarm_R, FParkourFNames::Bone_Hand_R });
		break;
	case EBodyPart::LeftLeg:
		outChain = TArray<FName>({ FParkourFNames::Bone_Thigh_L, FParkourFNames::Bone_Calf_L, FParkourFNames::Bone_Foot_L });
		break;
	case EBodyPart::RightLeg:
		outChain = TArray<FName>({ FParkourFNames::Bone_Thigh_R, FParkourFNames::Bone_Calf_R, FParkourFNames::Bone_Foot_R });
		break;
	case EBodyPart::Pelvis:
		outChain = TArray<FName>({ FParkourFNames::Bone_Pelvis });
		break;
	}
}


bool UParkourHelperLibrary::IsValidPlayerName(const FString& Name)
{
  if (Name.IsEmpty()) return false;

  const FRegexPattern myPattern(TEXT("^[a-z,A-Z,0-9,_]+"));
  FRegexMatcher myMatcher(myPattern, *Name);

  myMatcher.FindNext();

  return myMatcher.GetMatchBeginning() == 0 && (myMatcher.GetMatchEnding() == Name.Len());

}