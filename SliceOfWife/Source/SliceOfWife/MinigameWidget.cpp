// Fill out your copyright notice in the Description page of Project Settings.

#include "MinigameWidget.h"
#include "AssemblingSpot.h"

void UMinigameWidget::StartMinigame(AActor* initiator)
{
	minigameInitator = initiator;

	this->AddToViewport();
}

void UMinigameWidget::EndMinigame(bool isCompleted)
{
	if (minigameInitator->IsA(AAssemblingSpot::StaticClass()))
	{
		Cast<AAssemblingSpot>(minigameInitator)->AssembleBodyPart();
	}

	this->RemoveFromParent();
}
