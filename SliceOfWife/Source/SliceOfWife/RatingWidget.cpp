// Fill out your copyright notice in the Description page of Project Settings.

#include "RatingWidget.h"
#include "BodyStorage.h"
#include "AssemblingTable.h"
#include "FullBody.h"
#include "EnumsStructs.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

int URatingWidget::CalculateRating()
{
	float rating = (float)CountPlayerCreatureVariation() / RequiredCreatureTypes * StarCount;

	return (int)rating;
}

int URatingWidget::CountPlayerCreatureVariation()
{
	AActor* assemblingTable = UGameplayStatics::GetActorOfClass(GetWorld(), AAssemblingTable::StaticClass());
	AFullBody* playerCreature = Cast<AAssemblingTable>(assemblingTable)->FinalBody;

	return playerCreature->CountCreatureTypeVariation();
}
