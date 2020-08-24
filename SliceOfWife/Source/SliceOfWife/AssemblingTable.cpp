#include "AssemblingTable.h"
#include "MinigameWidget.h"
#include "Blueprint/UserWidget.h"
#include "Engine.h"
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

bool AAssemblingTable::DropToTable(AActor* objectToDrop)
{
	// get all of its scene components
	TArray<UActorComponent*> components;
	components = this->GetComponentsByClass(USceneComponent::StaticClass());

	// check each component
	for (int i = 0; i < components.Num(); ++i)
	{
		// check each component's tags
		for (int c = 0; c < components[i]->ComponentTags.Num(); ++c)
		{
			// check the dropped object's tags
			for (int d = 0; d < objectToDrop->Tags.Num(); ++d)
			{
				// if there is a matching tag
				if (components[i]->ComponentTags[c] == objectToDrop->Tags[d])
				{
					bool isMissingPart = true;

					// check the existing body parts' tags
					for (int b = 0; b < bodyParts.Num(); ++b)
					{
						// if there is already a body part with the same tag
						if (objectToDrop->Tags[d] == bodyParts[b].tag)
						{
							isMissingPart = false;
						}
					}

					if (isMissingPart)
					{
						// cast the component as a scene component
						USceneComponent* sceneComponent = Cast<USceneComponent>(components[i]);

						// snap the dropped object to the component
						objectToDrop->SetActorRotation(FRotator(0, 0, 0), ETeleportType::ResetPhysics);
						objectToDrop->AttachToComponent(sceneComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
						objectToDrop->SetActorLocation(sceneComponent->GetComponentLocation());

						// remember the dropped body part
						ObjectOnTable bodyPart;
						bodyPart.object = objectToDrop;
						bodyPart.tag = objectToDrop->Tags[d];
						bodyParts.Add(bodyPart);

						return true;
					}
				}
			}
		}
	}

	return false;
}

bool AAssemblingTable::RemoveFromTable(AActor* objectToRemove)
{
	// detach the object from the table
	objectToRemove->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	
	// check the list of body parts
	for (int i = 0; i < bodyParts.Num(); ++i)
	{
		// remove the body part from the list
		if (objectToRemove == bodyParts[i].object)
		{
			bodyParts.RemoveAt(i);
			return true;
		}
	}

	return false;
}

void AAssemblingTable::StartMinigame()
{
	if (WidgetBP != nullptr)
	{
		widget = CreateWidget<UMinigameWidget>(GetWorld(), WidgetBP.Get());
		widget->StartMinigame(this);
	}
}

void AAssemblingTable::Assemble(ABodyPart* bodyPArt)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Success!")));
}

bool AAssemblingTable::Animate()
{
	if (bodyParts.Num() < MinBodyParts || TemporarySpawnBody == nullptr)
	{
		return false;
	}

	// destroy each body part
	for (int i = 0; i < bodyParts.Num(); ++i)
	{
		bodyParts[i].object->Destroy();
	}
	bodyParts.Empty();

	// spawn the body
	FVector location(this->GetActorLocation() + SpawnOffset);
	FRotator rotation(this->GetActorRotation() + SpawnRotation);
	FActorSpawnParameters spawnInfo;
	GetWorld()->SpawnActor<AActor>(TemporarySpawnBody, location, rotation, spawnInfo);

	return false;
}

