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

private:

	bool gachaInitialized = false;

public:

	UFUNCTION(BlueprintCallable)
	void InitializeGachaSystem();

	UFUNCTION(BlueprintCallable)
	FGachaItem RollGacha(EGachaItemType itemType);

private:

	void SortGachaPool(TArray<FGachaRarityPool>* gachaPool);

	EGachaRarity DetermineRarity(EGachaItemType itemType);
};
