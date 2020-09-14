// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RatingWidget.generated.h"

/**
 * 
 */
UCLASS()
class SLICEOFWIFE_API URatingWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite)
	int Rating = 0;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "1"))
	int StarCount = 5;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "1"))
	int RequiredCreatureTypes = 2;

	UFUNCTION(BlueprintCallable)
	int CalculateRating();

private:

	int CountPlayerCreatureVariation();
};
