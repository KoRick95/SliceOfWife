// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MinigameWidget.h"
#include "FishingWidget.generated.h"

/**
 * 
 */
UCLASS()
class SLICEOFWIFE_API UFishingWidget : public UMinigameWidget
{
	GENERATED_BODY()
	
public:

	void ListenForAxisInput();
};
