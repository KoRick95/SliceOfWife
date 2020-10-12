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

	//BoxCollider = CreateDefaultSubobject<UBoxComponent>("Fuckmeamirite");
	//BoxCollider->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
}

// Called when the game starts or when spawned
void AAssemblingSpot::BeginPlay()
{
	Super::BeginPlay();

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
}

// Called every frame
void AAssemblingSpot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool AAssemblingSpot::SetBodyPart(ABodyPart* aBodyPart)
{
	if (aBodyPart != nullptr)
	{
		TArray<EBodyPartType> currentTypes = aBodyPart->GetCurrentMeshTypes();

		for (int i = 0; i < currentTypes.Num(); ++i)
		{
			if (currentTypes[i] == this->BodyPartType)
			{
				bodyPart = aBodyPart;
				return true;
			}
		}
	}

	return false;
}

bool AAssemblingSpot::DropToTable(AActor* object)
{
	return table->DropToTable(object, this);
}

bool AAssemblingSpot::RemoveFromTable(ABodyPart* aBodyPart)
{
	return table->RemoveFromTable(aBodyPart);
}

bool AAssemblingSpot::BeginSewing()
{
	return table->BeginSewing(this);
}

void AAssemblingSpot::AssembleBodyPart()
{
	table->AssembleBodyPart(bodyPart);
}

bool AAssemblingSpot::IsOccupied()
{
	return bodyPart != nullptr;
}