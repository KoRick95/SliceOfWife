// Fill out your copyright notice in the Description page of Project Settings.

#include "Soul.h"
#include "BodyPart.h"
#include "Components/PrimitiveComponent.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Math/UnrealMathUtility.h"
#include "UObject/WeakObjectPtrTemplates.h"

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

	if (DelaySpawn)
	{
		SpawnDelayMax = (SpawnDelayMin > SpawnDelayMax) ? SpawnDelayMin : SpawnDelayMax;

		// despawn immediately
		Despawn();

		// set a random delay timer
		spawnTimer = FMath::FRandRange(SpawnDelayMin, SpawnDelayMax);
	}
	
	// calculate a random direction
	direction = GetRandomDirection();
}

// Called every frame
void ASoul::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!IsValid(possession))
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, FString::Printf(TEXT("A soul lost its possession.")));
		this->Destroy();
	}

	if (hasSpawned)
	{
		// move towards a certain direction
		this->AddActorWorldOffset(direction * MoveSpeed * GetWorld()->GetDeltaSeconds());

		FVector position = this->GetActorLocation();

		// if the soul has crossed the edge of the map
		if (position.X < -MapEdgeX || position.X > MapEdgeX || position.Y < -MapEdgeY || position.Y > MapEdgeY)
		{
			possession->Destroy();
			this->Destroy();
		}
	}
	else
	{
		// if the object isn't attached to anything
		if (possession->GetAttachParentActor() == nullptr)
		{
			// count down the spawn timer
			spawnTimer -= GetWorld()->GetDeltaSeconds();

			if (spawnTimer < 0)
			{
				Spawn();
			}
		}
	}
}

FVector ASoul::GetRandomDirection()
{
	float x = FMath::FRandRange(-1, 1);
	float y = direction.Y = FMath::Sqrt(1 - FMath::Square(direction.X)) * FMath::Pow(-1, FMath::RandRange(1, 2));
	float z = 0;
	
	return FVector(x, y, z);
}

void ASoul::HoldObject()
{
	// disable physics
	TArray<UActorComponent*> primitiveComponents = possession->GetComponentsByClass(UPrimitiveComponent::StaticClass());
	for (int i = 0; i < primitiveComponents.Num(); ++i)
	{
		Cast<UPrimitiveComponent>(primitiveComponents[i])->SetSimulatePhysics(false);
	}

	// attach the object to the soul
	possession->AttachToActor(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	possession->SetActorRelativeLocation(-Cast<ABodyPart>(possession)->GetMeshRelativeLocation());
}

void ASoul::ReleaseObject()
{
	if (possession == nullptr)
		return;

	// detach object from the soul
	possession->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	// enable physics
	TArray<UActorComponent*> primitiveComponents = possession->GetComponentsByClass(UPrimitiveComponent::StaticClass());
	for (int i = 0; i < primitiveComponents.Num(); ++i)
	{
		Cast<UPrimitiveComponent>(primitiveComponents[i])->SetSimulatePhysics(true);
	}
}

void ASoul::Spawn()
{
	float rng = FMath::FRandRange(0, 1);

	// roll for a chance to spawn
	if (rng > SpawnChance)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, FString::Printf(TEXT("Failed chance to spawn."), rng));
		if (CanRespawn)
		{
			spawnTimer = RespawnTime;
			return;
		}
		else
		{
			this->Destroy();
		}
	}

	// set the manifest location
	FVector newPosition = possession->GetActorLocation(); 
	newPosition.Z = FloatHeight;
	this->SetActorLocation(newPosition, false, nullptr, ETeleportType::TeleportPhysics);

	// turn on visibility
	SetActorHiddenInGame(false);

	// enable collision
	SetActorEnableCollision(true);

	// attach object
	HoldObject();

	hasSpawned = true;
}

void ASoul::Despawn()
{
	// turn off visibility
	SetActorHiddenInGame(true);

	// disable collision
	SetActorEnableCollision(false);

	// detach object
	ReleaseObject();

	if (CanRespawn)
	{
		spawnTimer = RespawnTime;
	}
	else
	{
		this->Destroy();
	}

	hasSpawned = false;
}