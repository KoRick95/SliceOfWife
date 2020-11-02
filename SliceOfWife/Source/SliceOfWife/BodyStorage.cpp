#include "BodyStorage.h"
#include "Creature.h"
#include "Engine.h"
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
	if (MaxBodyCount > 0 && CreaturePool.Num() > 0)
	{
		// if the number of used creatures have reached max
		if (currentBodies.Num() >= MaxBodyCount)
		{
			// if permanent limit is on
			if (bPermamentLimit)
			{
				return nullptr;
			}
			else
			{
				// if the oldest creature in the list is valid
				if (IsValid(currentBodies[0]))
				{
					// destroy it and render it invalid
					currentBodies[0]->Destroy();
				}

				// remove the invalid creature from the list
				currentBodies.RemoveAt(0);
			}
		}

		// randomise the body being created
		int i = FMath::RandRange(0, CreaturePool.Num() - 1);

		// set the body parameters
		UClass* uClass = CreaturePool[i].Get();
		FTransform transform = CreaturePool[i].GetDefaultObject()->GetActorTransform();
		FActorSpawnParameters spawnParams;
		spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		// spawn a new body and add it to the array
		AActor* newBody = GetWorld()->SpawnActor(uClass, &transform, spawnParams);
		currentBodies.Add(newBody);

		return newBody;
	}

	return nullptr;
}
