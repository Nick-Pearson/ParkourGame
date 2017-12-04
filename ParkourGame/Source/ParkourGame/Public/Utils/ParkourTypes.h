#pragma once

UENUM(BlueprintType)
enum class EBodyPart : uint8
{
	Head,
	Torso,
	LeftArm,
	RightArm,
	Pelvis,
	LeftLeg,
	RightLeg,

	MAX			UMETA(Hidden)
};