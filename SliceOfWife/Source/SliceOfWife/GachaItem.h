// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EnumsStructs.h"
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GachaItem.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class SLICEOFWIFE_API UGachaItem : public UObject
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere)
	FName Name;

	UPROPERTY(EditAnywhere)
	FString Description;

	UPROPERTY(EditAnywhere)
	TEnumAsByte<EGachaItemType> ItemType;

	UPROPERTY(EditAnywhere)
	TEnumAsByte<EGachaRarity> Rarity;
};
