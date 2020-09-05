#pragma once

#include "EnumsStructs.generated.h"

UENUM()
enum EBodyPartType
{
	None		UMETA(DisplayName = "None"),
	Head		UMETA(DisplayName = "Head"),
	Torso		UMETA(DisplayName = "Torso"),
	LeftArm		UMETA(DisplayName = "Left Arm"),
	RightArm	UMETA(DisplayName = "Right Arm"),
	LeftLeg		UMETA(DisplayName = "Left Leg"),
	RightLeg	UMETA(DisplayName = "Right Leg"),
	LeftWing	UMETA(DisplayName = "Left Wing"),
	RightWing	UMETA(DisplayName = "Right Wing"),
	Tail		UMETA(DisplayName = "Tail"),
};

UENUM()
enum ECreatureType
{
	Unknown		UMETA(DisplayName = "Unknown"),
	HumanM		UMETA(DisplayName = "Human (Male)"),
	HumanF		UMETA(DisplayName = "Human (Female)"),
	Bird		UMETA(DisplayName = "Bird"),
	Bull		UMETA(DisplayName = "Bull"),
	Fish		UMETA(DisplayName = "Fish"),
	Snake		UMETA(DisplayName = "Snake"),
};

USTRUCT()
struct FObjectReplacement
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> Input;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> Output;
};

USTRUCT()
struct FBodyPartMeshType
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
	TEnumAsByte<EBodyPartType> BodyPartType = EBodyPartType::None;

	UPROPERTY(EditAnywhere)
	USkeletalMesh* SkeletalMesh = nullptr;
};