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
}

// Called every frame
void AAssemblingSpot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool AAssemblingSpot::BeginSewing()
{
	for (int i = 0; i < table->bodyParts.Num(); ++i)
	{
		if (this->ActorHasTag(table->bodyParts[i].tag))
		{
			
			return true;
		}
	}
	return false;
}

