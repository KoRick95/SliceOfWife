// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimatingDevice.h"
#include "AssemblingSpot.h"
#include "AssemblingTable.h"
#include "BodyPart.h"
#include "FullBody.h"
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
	if (assemblingTable == nullptr || assemblingTable->finalBody == nullptr)
	{
		return false;
	}

	TArray<ABodyPart*> uncheckedBodyParts = assemblingTable->finalBody->bodyParts;
	bool allRequirementsMet = true;

	if (RequiredBodyPartTypes.Num() <= uncheckedBodyParts.Num())
	{
		for (int r = 0; r < RequiredBodyPartTypes.Num(); ++r)
		{
			bool requirementMet = false;

			for (int u = 0; u < uncheckedBodyParts.Num(); ++u)
			{
				if (RequiredBodyPartTypes[r] == uncheckedBodyParts[u]->GetBodyPartType())
				{
					uncheckedBodyParts.RemoveAt(u);
					requirementMet = true;
					break;
				}
			}

			if (!requirementMet)
			{
				allRequirementsMet = false;
				break;
			}
		}
	}

	if (allRequirementsMet)
	{
		assemblingTable->AnimateBody();
		Animated = true;
	}

	return allRequirementsMet;
}
