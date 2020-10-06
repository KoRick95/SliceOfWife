#include "BodyStorage.h"
#include "Creature.h"
#include "Engine/World.h"

// Sets default values
ABodyStorage::ABodyStorage()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ABodyStorage::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ABodyStorage::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

AActor* ABodyStorage::TakeBody()
{
	// if there is no body in the storage
	if (Bodies.Num() < 1)
	{
		return nullptr;
	}

	bool canCreateNewBody = true;

	//// if the player already has the max allowable number of bodies from this storage
	//if (bodyCount == MaxBodyCount)
	//{
	//	canCreateNewBody = false;

	//	// while the player cannot create a new body and there are still bodies to check through
	//	while (!canCreateNewBody && currentBodies.Num() > 0)
	//	{
	//		// if the body has not been destroyed
	//		if (IsValid(currentBodies[0]))
	//		{
	//			// destroy the oldest body in the array and allow the player to create a new body
	//			currentBodies[0]->Destroy();
	//			bodyCount--;
	//			canCreateNewBody = true;
	//		}

	//		// remove the body from the array
	//		currentBodies.RemoveAt(0);
	//	}
	//}

	if (canCreateNewBody)
	{
		// randomise the body being created
		int i = FMath::RandRange(0, Bodies.Num() - 1);

		// set the body parameters
		UClass* uClass = Bodies[i].Get();
		FTransform transform = Bodies[i].GetDefaultObject()->GetActorTransform();
		FActorSpawnParameters spawnParams;

		// spawn a new body and add it to the array
		AActor* newBody = GetWorld()->SpawnActor(uClass, &transform, spawnParams);
		currentBodies.Add(newBody);
		bodyCount++;

		return newBody;
	}

	return nullptr;
}
