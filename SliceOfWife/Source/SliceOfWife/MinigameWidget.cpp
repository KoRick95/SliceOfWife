// Fill out your copyright notice in the Description page of Project Settings.


#include "MinigameWidget.h"

void UMinigameWidget::StartMinigame(AActor* initiator)
{
	minigameInitator = initiator;

	this->AddToViewport();
}

void UMinigameWidget::EndMinigame(bool isCompleted)
{
	GameCompleted = isCompleted;

	this->RemoveFromParent();
}
