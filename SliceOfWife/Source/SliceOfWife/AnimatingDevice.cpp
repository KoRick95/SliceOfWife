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

bool AAnimatingDevice::BeginAnimating()
{
	if (CanAnimate())
	{
		if (MinigameWidget)
		{
			CreateWidget<UMinigameWidget>(GetWorld(), MinigameWidget.Get())->StartMinigame(this);
		}
		else
		{
			AnimateBody();
		}

		return true;
	}

	return false;
}

void AAnimatingDevice::AnimateBody()
{
	bool isAnimated = assemblingTable->AnimateBody();

	Animated = true;
}

bool AAnimatingDevice::CanAnimate()
{
	if (!Animated)
	{
		if (assemblingTable != nullptr && assemblingTable->FinalBody != nullptr)
		{
			TArray<ABodyPart*> currentBodyParts = assemblingTable->FinalBody->bodyParts;
			TArray<EBodyPartType> currentMeshTypes;

			// get all of the sewn body part types
			for (int b = 0; b < currentBodyParts.Num(); ++b)
			{
				currentMeshTypes.Append(currentBodyParts[b]->GetCurrentMeshTypes());
			}

			// if there are at least as many sewn body part types as the required body part types
			if (RequiredBodyParts.Num() <= currentMeshTypes.Num())
			{
				// check all the required body part types
				for (int r = 0; r < RequiredBodyParts.Num(); ++r)
				{
					bool requirementMet = false;

					// check all the sewn body part types
					for (int c = 0; c < currentMeshTypes.Num(); ++c)
					{
						// if the sewn body part type and the required body part type matches
						if (RequiredBodyParts[r] == currentMeshTypes[c])
						{
							requirementMet = true;

							// remove the type from the checklist
							currentMeshTypes.RemoveAt(c);

							// move on to the next required body part type
							break;
						}
					}

					// if one of the requirement is not met, exit and return false
					if (!requirementMet)
					{
						return false;
					}
				}

				// if the check executed without exiting, then all the requirements are met
				return true;
			}
		}
	}

	return false;
}
