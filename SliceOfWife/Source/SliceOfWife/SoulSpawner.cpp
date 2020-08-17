// Fill out your copyright notice in the Description page of Project Settings.

#include "SoulSpawner.h"
#include "Soul.h"
#include "Engine/World.h"

// Sets default values
ASoulSpawner::ASoulSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ASoulSpawner::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ASoulSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASoulSpawner::SpawnSoul()
{
	if (soul != nullptr)
	{
		UClass* uClass = soul->GetClass();
		FVector position = hauntedObject->GetActorLocation();
		FRotator rotation(0);
		FActorSpawnParameters spawnParams;

		GetWorld()->SpawnActor(uClass, &position, &rotation, spawnParams);

		this->Destroy();
	}
}