// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EnumsStructs.h"
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GachaPool.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class SLICEOFWIFE_API UGachaPool : public UObject
{
	GENERATED_BODY()
	
public:

	UGachaPool();

	UPROPERTY(EditAnywhere)
	FName GachaName;

	UPROPERTY(EditAnywhere)
	FGachaRarityPool Common;
	
	UPROPERTY(EditAnywhere)
	FGachaRarityPool Rare;

	UPROPERTY(EditAnywhere)
	FGachaRarityPool SuperRare;

	void Initialize();

	UGachaItem* RollItem();

private:

	TArray<FGachaRarityPool*> gachaPool;

	float GetTotalProbability();

	FGachaRarityPool* RollRarityPool();
};
