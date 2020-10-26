// Fill out your copyright notice in the Description page of Project Settings.

#include "ShopWidget.h"
#include "GachaItem.h"
#include "GachaPool.h"

void UShopWidget::OnInitialized()
{
	for (int i = 0; i < GachaBlueprints.Num(); ++i)
	{
		UGachaPool* gachaPool = GachaBlueprints[i].GetDefaultObject();

		if (gachaPool)
		{
			gachaPool->Initialize();
			GachaPools.Add(gachaPool);
		}
	}
}

UGachaItem* UShopWidget::RollGachaByIndex(int gachaIndex)
{
	if (gachaIndex >= 0 && gachaIndex < GachaPools.Num())
	{
		return GachaPools[gachaIndex]->RollItem();
	}

	return nullptr;
}

UGachaItem* UShopWidget::RollGachaByName(FName gachaName)
{
	for (int i = 0; i < GachaPools.Num(); ++i)
	{
		if (GachaPools[i]->GachaName == gachaName)
		{
			return GachaPools[i]->RollItem();
		}
	}
	
	return nullptr;
}
