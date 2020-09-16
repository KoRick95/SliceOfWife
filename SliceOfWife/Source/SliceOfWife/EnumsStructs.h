#pragma once

#include "EnumsStructs.generated.h"

UENUM()
enum EBodyPartType
{
	Head		UMETA(DisplayName = "Head"),
	Torso		UMETA(DisplayName = "Torso"),
	LeftArm		UMETA(DisplayName = "Left Arm"),
	RightArm	UMETA(DisplayName = "Right Arm"),
	LeftLeg		UMETA(DisplayName = "Left Leg"),
	RightLeg	UMETA(DisplayName = "Right Leg"),
	LeftWing	UMETA(DisplayName = "Left Wing"),
	RightWing	UMETA(DisplayName = "Right Wing"),
	Tail		UMETA(DisplayName = "Tail"),
	Other		UMETA(DisplayName = "Other"),
};

UENUM()
enum ECreatureType
{
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
enum EGachaItemType
{
	Skin		UMETA(DisplayName = "Skin"),
	BodyPart	UMETA(DisplayName = "Body Part"),
	Misc		UMETA(DisplayName = "Miscellaneous")
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

USTRUCT(BlueprintType)
struct FGachaItem
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TEnumAsByte<EGachaItemType> Type;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TEnumAsByte<EGachaRarity> Rarity;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AActor> Item;
};

USTRUCT()
struct FGachaRarityPool
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
	TEnumAsByte<EGachaRarity> Rarity;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0"))
	float ProbabilityValue = 0;

	UPROPERTY(EditAnywhere)
	TArray<FGachaItem> Items;
};