// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EnumsStructs.h"
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ShopWidget.generated.h"

/**
 * 
 */
UCLASS()
class SLICEOFWIFE_API UShopWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere)
	TArray<FGachaRarityPool> GachaSkinPool;

	UPROPERTY(EditAnywhere)
	TArray<FGachaRarityPool> GachaBodyPartPool;

public:

	UFUNCTION(BlueprintCallable)
	FGachaItem RollGacha(EGachaItemType itemType);

private:

	EGachaRarity DetermineRarity(EGachaItemType itemType);
};
