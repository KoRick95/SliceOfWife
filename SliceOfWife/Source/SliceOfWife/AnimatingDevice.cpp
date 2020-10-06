// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimatingDevice.h"
#include "AssemblingSpot.h"
#include "AssemblingTable.h"
#include "BodyPart.h"
#include "Creature.h"
#include "MinigameWidget.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AAnimatingDevice::AAnimatingDevice()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AAnimatingDevice::BeginPlay()
{
	Super::BeginPlay();

	assemblingTable = Cast<AAssemblingTable>(UGameplayStatics::GetActorOfClass(this, AAssemblingTable::StaticClass()));
}

// Called every frame
void AAnimatingDevice::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool AAnimatingDevice::AnimateBody()
{
	if (assemblingTable == nullptr || assemblingTable->FinalBody == nullptr)
	{
		return false;
	}

	TArray<TEnumAsByte<EBodyPartType>> missingBodyParts = RequiredBodyParts;
	TArray<ABodyPart*> currentBodyParts = assemblingTable->FinalBody->bodyParts;
	TArray<EBodyPartType> currentMeshTypes;
	bool requirementsMet = true;

	for (int b = 0; b < currentBodyParts.Num(); ++b)
	{
		currentMeshTypes.Append(currentBodyParts[b]->GetCurrentMeshTypes());
	}

	if (RequiredBodyParts.Num() <= currentMeshTypes.Num())
	{
		for (int r = 0; r < RequiredBodyParts.Num(); ++r)
		{
			requirementsMet = false;

			for (int c = 0; c < currentMeshTypes.Num(); ++c)
			{
				if (RequiredBodyParts[r] == currentMeshTypes[c])
				{
					requirementsMet = true;
					break;
				}
			}

			if (!requirementsMet)
			{
				return false;
			}
		}
	}

	if (requirementsMet)
	{
		if (MinigameWidget != nullptr)
		{
			CreateWidget<UMinigameWidget>(GetWorld(), MinigameWidget.Get())->StartMinigame(this);
		}
		
		assemblingTable->AnimateBody();
		Animated = true;
	}

	return requirementsMet;
}
