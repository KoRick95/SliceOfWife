#include "AssemblingTable.h"
#include "Engine/World.h"

// Sets default values
AAssemblingTable::AAssemblingTable()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AAssemblingTable::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAssemblingTable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool AAssemblingTable::DropToTable(AActor* droppedObject)
{
	// get all of its scene components
	TArray<UActorComponent*> components;
	components = this->GetComponentsByClass(USceneComponent::StaticClass());

	// check the dropped object's tags
	for (int i = 0; i < droppedObject->Tags.Num(); ++i)
	{
		// check the existing body parts' tags
		for (int j = 0; j < bodyPartTags.Num(); ++j)
		{
			// if there is already a body part with the same tag
			if (droppedObject->Tags[i] == bodyPartTags[j])
			{
				// do something? maybe replace the body part or simply deny the dropping
			}
		}
	}

	// check each component
	for (int i = 0; i < components.Num(); ++i)
	{
		// check each component's tags
		for (int c = 0; c < components[i]->ComponentTags.Num(); ++c)
		{
			// check the dropped object's tags
			for (int d = 0; d < droppedObject->Tags.Num(); ++d)
			{
				// if there is a matching tag
				if (components[i]->ComponentTags[c] == droppedObject->Tags[d])
				{
					// cast the component as a scene component
					USceneComponent* sceneComponent = Cast<USceneComponent>(components[i]);

					// snap the dropped object to the component
					droppedObject->SetActorRotation(FRotator(0, 0, 0), ETeleportType::ResetPhysics);
					droppedObject->AttachToComponent(sceneComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
					droppedObject->SetActorLocation(sceneComponent->GetComponentLocation());
					bodyPartTags.Add(droppedObject->Tags[d]);
					return true;
				}
			}
		}
	}

	return false;
}

bool AAssemblingTable::Animate()
{
	if (bodyPartTags.Num() < MinBodyParts)
	{
		return false;
	}

	if (TemporarySpawnBody == nullptr)
	{
		return false;
	}

	FVector Location(this->GetActorLocation() + SpawnOffset);
	FRotator Rotation(this->GetActorRotation());
	FActorSpawnParameters SpawnInfo;
	GetWorld()->SpawnActor<AActor>(TemporarySpawnBody, Location, Rotation, SpawnInfo);

	return false;
}

