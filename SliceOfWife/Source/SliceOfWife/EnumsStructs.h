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
	Bat			UMETA(DisplayName = "Bat"),
	Bird		UMETA(DisplayName = "Bird"),
	Bull		UMETA(DisplayName = "Bull"),
	Fish		UMETA(DisplayName = "Fish"),
	Snake		UMETA(DisplayName = "Snake"),
	Custom		UMETA(DisplayName = "Custom"),
};

UENUM()
enum EGachaRarity
{
	Common		UMETA(DisplayName = "Common"),
	Rare		UMETA(DisplayName = "Rare"),
	SuperRare	UMETA(DisplayName = "Super Rare"),
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
struct FBodyPartMesh
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
	class USkeletalMesh* SkeletalMesh = nullptr;

	UPROPERTY(EditAnywhere)
	TArray<TEnumAsByte<EBodyPartType>> BodyPartTypes;
};

USTRUCT()
struct FGachaRarityValue
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
	TEnumAsByte<EGachaRarity> Rarity;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0"))
	float ProbabilityValue = 0;
};

USTRUCT()
struct FGachaItem
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> Item;

	UPROPERTY(EditAnywhere)
	TEnumAsByte<EGachaRarity> Rarity;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0"))
	float ProbabilityValue = 0;
};