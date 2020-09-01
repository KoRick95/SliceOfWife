#pragma once

#include "Commons.generated.h"

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

USTRUCT()
struct FBodyPartReplacement
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
	TSubclassOf<class ABodyPart> input;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class ABodyPart> output;
};