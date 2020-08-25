// Fill out your copyright notice in the Description page of Project Settings.


#include "AssemblingSpot.h"
#include "AssemblingTable.h"
#include "BodyPart.h"
#include "Engine.h"

// Sets default values
AAssemblingSpot::AAssemblingSpot()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AAssemblingSpot::BeginPlay()
{
	Super::BeginPlay();

	if (table == nullptr)
	{
		if (this->GetAttachParentActor() != nullptr)
		{
			if (this->GetAttachParentActor()->IsA(AAssemblingTable::StaticClass()))
			{
				table = Cast<AAssemblingTable>(this->GetAttachParentActor());
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("An assembling spot is not attached to a table.")));
			}
		}
	}

	if (Tags.Num() == 0)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("An assembling spot is not tagged.")));
	}

	TArray<UActorComponent*> tableSceneComponents = table->GetComponentsByClass(USceneComponent::StaticClass());
	for (int i = 0; i < tableSceneComponents.Num(); ++i)
	{
		for (int j = 0; j < tableSceneComponents[i]->ComponentTags.Num(); ++j)
		{
			if (ActorHasTag(tableSceneComponents[i]->ComponentTags[j]))
			{
				snapComponent = Cast<USceneComponent>(tableSceneComponents[i]);
			}
		}
	}

	if (snapComponent == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("An assembling spot cannot find a matching snap point.")));
	}
}

// Called every frame
void AAssemblingSpot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool AAssemblingSpot::DropBodyPart(ABodyPart* aBodyPart)
{
	if (aBodyPart->ActorHasTag(table->CentralBodyPartTag))
	{
		
	}

	if (bodyPart == nullptr)
	{
		for (int i = 0; i < aBodyPart->Tags.Num(); ++i)
		{
			if (this->ActorHasTag(aBodyPart->Tags[i]))
			{
				if (snapComponent != nullptr)
				{
					// snap the dropped object to the component
					aBodyPart->SetActorRotation(FRotator(0, 0, 0), ETeleportType::ResetPhysics);
					aBodyPart->AttachToComponent(snapComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
					aBodyPart->SetActorLocation(snapComponent->GetComponentLocation());

					bodyPart = aBodyPart;
					return true;
				}
			}
		}
	}
	
	return false;
}

bool AAssemblingSpot::RemoveBodyPart()
{
	if (bodyPart != nullptr)
	{
		bodyPart->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		bodyPart = nullptr;
		return true;
	}

	return false;
}

bool AAssemblingSpot::BeginSewing()
{
	for (int i = 0; i < table->bodyPartsOnTable.Num(); ++i)
	{
		if (this->ActorHasTag(table->bodyPartsOnTable[i].tag))
		{
			
			return true;
		}
	}
	return false;
}

