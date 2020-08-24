// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MinigameHUD.generated.h"

/**
 *
 */
UCLASS()
class SLICEOFWIFE_API AMinigameHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	AMinigameHUD();

	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> WidgetBP;

private:
	class UUserWidget* widget;
};
