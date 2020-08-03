#include "BodyStorage.h"
#include "Engine/World.h"

// Sets default values
ABodyStorage::ABodyStorage()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Tags.Add("BodyStorage");
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

AActor* ABodyStorage::TakeBodyFrom()
{
	// if there is no body in the storage
	if (Bodies.Num() < 1)
	{
		return nullptr;
	}

	// if the player already took a body out
	if (currentBody != nullptr)
	{
		// delete the previous body
		currentBody->Destroy();
		currentBody = nullptr;
	}

	// spawn a random body
	int i = FMath::RandRange(0, Bodies.Num() - 1);
	currentBody = GetWorld()->SpawnActor<AActor>(Bodies[i]);

	return currentBody;
}
