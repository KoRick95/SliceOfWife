// Fill out your copyright notice in the Description page of Project Settings.

#include "Soul.h"
#include "Components/PrimitiveComponent.h"
#include "Engine/World.h"
#include "Math/UnrealMathUtility.h"

// Sets default values
ASoul::ASoul()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ASoul::BeginPlay()
{
	Super::BeginPlay();

	// calculate a random direction
	direction.X = FMath::FRandRange(-1, 1);
	direction.Y = FMath::Sqrt(1 - FMath::Square(direction.X)) * FMath::Pow(-1, FMath::RandRange(1, 2));
}

// Called every frame
void ASoul::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// move towards a certain direction
	this->AddActorWorldOffset(direction * MoveSpeed * GetWorld()->GetDeltaSeconds());

	FVector position = this->GetActorLocation();

	// if the soul has crossed the edge of the map
	if (position.X < -MapEdgeX || position.X > MapEdgeX || position.Y < -MapEdgeY || position.Y > MapEdgeY)
	{
		this->Destroy();
	}
}

void ASoul::HoldObject()
{
	// disable physics
	TArray<UActorComponent*> primitiveComponents = hauntedObject->GetComponentsByClass(UPrimitiveComponent::StaticClass());
	for (int i = 0; i < primitiveComponents.Num(); ++i)
	{
		Cast<UPrimitiveComponent>(primitiveComponents[i])->SetSimulatePhysics(false);
	}

	// attach the object to the soul
	hauntedObject->AttachToActor(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	hauntedObject->SetActorRelativeLocation(FVector(0));
}

void ASoul::ReleaseObject()
{

}

void ASoul::Appear()
{
	// turn on visibility
	SetActorHiddenInGame(false);

	// enable collision
	SetActorEnableCollision(true);

	// attach object
	HoldObject();
}

void ASoul::Disappear()
{
	// turn off visibility
	SetActorHiddenInGame(true);

	// disable collision
	SetActorEnableCollision(false);

	// detach object
	ReleaseObject();

	if (CanRespawn)
	{
		// reset timer
		spawnTimer = RespawnTime;
	}
}