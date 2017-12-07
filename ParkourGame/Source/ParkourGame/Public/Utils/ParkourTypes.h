#pragma once

UENUM(BlueprintType)
enum class EBodyPart : uint8
{
	Torso,
	LeftArm,
	RightArm,
	Pelvis,
	LeftLeg,
	RightLeg,
	Head,

	MAX			UMETA(Hidden)
};