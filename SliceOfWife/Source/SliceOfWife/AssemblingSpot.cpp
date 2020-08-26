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

	if (Tags.Num() == 0)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("An assembling spot is not tagged.")));
	}

	if (this->GetAttachParentActor() != nullptr)
	{
		if (this->GetAttachParentActor()->IsA(AAssemblingTable::StaticClass()))
		{
			table = Cast<AAssemblingTable>(this->GetAttachParentActor());
			
		}
	}

	if (table == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("An assembling spot is not attached to a table.")));
		return;
	}

	TArray<UActorComponent*> tableSceneComponents = table->GetComponentsByClass(USceneComponent::StaticClass());
	for (int i = 0; i < tableSceneComponents.Num(); ++i)
	{
		for (int j = 0; j < tableSceneComponents[i]->ComponentTags.Num(); ++j)
		{
			if (ActorHasTag(tableSceneComponents[i]->ComponentTags[j]))
			{
				tableComponent = Cast<USceneComponent>(tableSceneComponents[i]);
			}
		}
	}

	if (tableComponent == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("An assembling spot cannot find a matching snap point.")));
	}
}

// Called every frame
void AAssemblingSpot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool AAssemblingSpot::DropToTable(ABodyPart* aBodyPart)
{
	return table->DropToTableV2(aBodyPart, this);
}

bool AAssemblingSpot::RemoveFromTable(ABodyPart* aBodyPart)
{
	return table->RemoveFromTableV2(aBodyPart);
}

bool AAssemblingSpot::BeginSewing()
{
	return false;
}