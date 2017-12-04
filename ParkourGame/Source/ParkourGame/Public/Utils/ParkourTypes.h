#pragma once

UENUM(BlueprintType)
enum class EBodyPart : uint8
{
	Torso,
	LeftArm,
	RightArm,
	LeftLeg,
	RightLeg,
	Head,

	MAX			UMETA(Hidden)
};