// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MinigameWidget.generated.h"

/**
 * 
 */
UCLASS()
class SLICEOFWIFE_API UMinigameWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	AActor* minigameInitator = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool GameCompleted = false;

	UFUNCTION(BlueprintCallable)
	void StartMinigame(AActor* initiator = nullptr);

	UFUNCTION(BlueprintCallable)
	void EndMinigame(bool isCompleted = false);
};
