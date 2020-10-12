// Fill out your copyright notice in the Description page of Project Settings.

#include "MinigameWidget.h"
#include "AssemblingSpot.h"
#include "AnimatingDevice.h"

void UMinigameWidget::StartMinigame(AActor* initiator)
{
	if (initiator != nullptr)
	{
		minigameInitator = initiator;

		this->AddToViewport();
	}
}

void UMinigameWidget::EndMinigame(bool isCompleted)
{
	if (isCompleted)
	{
		if (minigameInitator->IsA(AAssemblingSpot::StaticClass()))
		{
			Cast<AAssemblingSpot>(minigameInitator)->AssembleBodyPart();
		}
		else if (minigameInitator->IsA(AAnimatingDevice::StaticClass()))
		{
			Cast<AAnimatingDevice>(minigameInitator)->AnimateBody();
		}
	}

	this->RemoveFromParent();
}
