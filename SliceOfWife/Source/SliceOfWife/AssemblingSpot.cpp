// Fill out your copyright notice in the Description page of Project Settings.


#include "AssemblingSpot.h"
#include "AssemblingTable.h"

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
			}
		}
	}
}

// Called every frame
void AAssemblingSpot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

