// Fill out your copyright notice in the Description page of Project Settings.

#include "Soul.h"
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
	FVector position = this->GetActorLocation();

	Super::Tick(DeltaTime);

	// move towards a certain direction
	this->AddActorWorldOffset(direction * moveSpeed * GetWorld()->GetDeltaSeconds());

	// if the soul has crossed the edge of the map
	if (position.X < -mapEdgeX || position.X > mapEdgeX || position.Y < -mapEdgeY || position.Y > mapEdgeY)
	{
		this->Destroy();
	}
}

void ASoul::HoldObject(AActor* object)
{
	object->AttachToActor(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	object->SetActorRelativeLocation(FVector(0));
}