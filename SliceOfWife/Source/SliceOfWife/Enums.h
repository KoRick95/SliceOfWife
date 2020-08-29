#pragma once

#include "Enums.generated.h"

UENUM()
enum EBodyPartType
{
	None		UMETA(DisplayName = "None"),
	Head		UMETA(DisplayName = "Head"),
	Torso		UMETA(DisplayName = "Torso"),
	LeftArm		UMETA(DisplayName = "LeftArm"),
	RightArm	UMETA(DisplayName = "RightArm"),
	LeftLeg		UMETA(DisplayName = "LeftLeg"),
	RightLeg	UMETA(DisplayName = "RightLeg"),
	Tail		UMETA(DisplayName = "Tail")
};