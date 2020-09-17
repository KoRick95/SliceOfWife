// Fill out your copyright notice in the Description page of Project Settings.

#include "ShopWidget.h"

FGachaItem UShopWidget::RollGacha(EGachaItemType itemType)
{
	TArray<FGachaItem>* itemPool = nullptr;
	EGachaRarity rarity = DetermineRarity(itemType);

	if (itemType == EGachaItemType::Skin)
	{
		for (int i = 0; i < GachaSkinPool.Num(); ++i)
		{
			if (GachaSkinPool[i].Rarity == rarity)
			{
				itemPool = &GachaSkinPool[i].Items;
				break;
			}
		}
	}
	else if (itemType == EGachaItemType::BodyPart)
	{
		for (int i = 0; i < GachaBodyPartPool.Num(); ++i)
		{
			if (GachaBodyPartPool[i].Rarity == rarity)
			{
				itemPool = &GachaBodyPartPool[i].Items;
				break;
			}
		}
	}

	if (itemPool != nullptr)
	{
		int index = FMath::RandRange(0, itemPool->Num());
		return (*itemPool)[index];
	}

	return FGachaItem();
}

EGachaRarity UShopWidget::DetermineRarity(EGachaItemType itemType)
{
	float totalProbability = 0;
	float accumulativeProbability = 0;
	TArray<FGachaRarityPool>* gachaPool;

	if (itemType == EGachaItemType::Skin)
	{
		gachaPool = &GachaSkinPool;
	}
	else if (itemType == EGachaItemType::BodyPart)
	{
		gachaPool = &GachaBodyPartPool;
	}
	else
	{
		return EGachaRarity();
	}

	for (int i = 0; i < gachaPool->Num(); ++i)
	{
		totalProbability += (*gachaPool)[i].ProbabilityValue;
	}

	float rng = FMath::FRandRange(0, totalProbability);

	for (int i = 0; i < gachaPool->Num(); ++i)
	{
		accumulativeProbability += (*gachaPool)[i].ProbabilityValue;

		if (accumulativeProbability > rng)
		{
			return (*gachaPool)[i].Rarity;
		}
	}

	return EGachaRarity();
}
