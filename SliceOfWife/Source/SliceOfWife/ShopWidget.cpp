// Fill out your copyright notice in the Description page of Project Settings.

#include "ShopWidget.h"
#include "GachaItem.h"
#include "GachaPool.h"
#include "Engine.h"

void UShopWidget::SetupShop()
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

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Gacha pool does not exist.")));
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
	
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Gacha pool does not exist.")));
	return nullptr;
}
