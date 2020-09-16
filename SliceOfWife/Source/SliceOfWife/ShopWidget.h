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
	TArray<FGachaRarityValue> RarityValues;

	UPROPERTY(EditAnywhere)
	TArray<FGachaItem> GachaSkins;

	UPROPERTY(EditAnywhere)
	TArray<FGachaItem> GachaBodyParts;

public:


};
