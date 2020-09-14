// Fill out your copyright notice in the Description page of Project Settings.

#include "FishingWidget.h"
#include "Engine/World.h"
#include "GameFramework/PlayerInput.h"
#include "Kismet/GameplayStatics.h"

void UFishingWidget::ListenForAxisInput()
{
	APlayerController* playerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	UPlayerInput* playerInput = playerController->PlayerInput;
	
	FInputAxisKeyMapping inputAxisKeyMapping;
	inputAxisKeyMapping.AxisName = "killme";
	playerInput->AddAxisMapping(inputAxisKeyMapping);

	
}