// Fill out your copyright notice in the Description page of Project Settings.


#include "GachaPool.h"
#include "GachaItem.h"

UGachaPool::UGachaPool()
{
	Common.Rarity = EGachaRarity::Common;
	Rare.Rarity = EGachaRarity::Rare;
	SuperRare.Rarity = EGachaRarity::SuperRare;
}

void UGachaPool::Initialize()
{
	gachaPool.Add(&Common);
	gachaPool.Add(&Rare);
	gachaPool.Add(&SuperRare);
}

float UGachaPool::GetTotalProbability()
{
	float totalProbability = 0;

	for (int i = 0; i < gachaPool.Num(); ++i)
	{
		totalProbability += gachaPool[i]->ProbabilityValue;
	}

	return totalProbability;
}

FGachaRarityPool* UGachaPool::RollRarityPool()
{
	float totalProbability = GetTotalProbability();
	float rng = FMath::FRandRange(0, totalProbability);
	float currentProbability = 0;

	for (int i = 0; i < gachaPool.Num(); ++i)
	{
		currentProbability += gachaPool[i]->ProbabilityValue;

		if (currentProbability > rng)
		{
			return gachaPool[i];
		}
	}

	return nullptr;
}

UGachaItem* UGachaPool::RollItem()
{
	FGachaRarityPool* itemPool = RollRarityPool();

	if (itemPool)
	{
		float rng = FMath::RandRange(0, itemPool->ItemBlueprints.Num() - 1);

		return itemPool->ItemBlueprints[rng].GetDefaultObject();
	}

	return nullptr;
}
