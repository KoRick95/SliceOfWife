#include "AssemblingTable.h"
#include "AssemblingSpot.h"
#include "BodyPart.h"
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

	TArray<AActor*> children;
	this->GetAllChildActors(children);

	// add any assembling spots attached to the table
	for (int i = 0; i < children.Num(); ++i)
	{
		if (children[i]->IsA(AAssemblingSpot::StaticClass()))
		{
			this->assemblingSpots.Add(Cast<AAssemblingSpot>(children[i]));
		}
	}

	TArray<UActorComponent*> sceneComponents;
	sceneComponents = GetComponentsByClass(USceneComponent::StaticClass());

	for (int i = 0; i < sceneComponents.Num(); ++i)
	{
		if (sceneComponents[i]->ComponentHasTag(CentralBodyPartTag))
		{
			CentralComponent = Cast<USceneComponent>(sceneComponents[i]);
		}
	}

	if (CentralComponent == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("The disassembling table has no central component.")));
	}
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
					for (int b = 0; b < bodyPartsOnTable.Num(); ++b)
					{
						// if there is already a body part with the same tag
						if (objectToDrop->Tags[d] == bodyPartsOnTable[b].tag)
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
						bodyPartsOnTable.Add(bodyPart);

						return true;
					}
				}
			}
		}
	}

	return false;
}

bool AAssemblingTable::DropToTableV2(ABodyPart* bodyPart, AAssemblingSpot* spot)
{
	if (bodyPart == nullptr || spot == nullptr)
	{
		return false;
	}

	if (bodyPart->ActorHasTag(CentralBodyPartTag))
	{
		// snap the dropped object to the central component
		bodyPart->SetActorRotation(FRotator(0, 0, 0), ETeleportType::ResetPhysics);
		bodyPart->AttachToComponent(CentralComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		bodyPart->SetActorLocation(CentralComponent->GetComponentLocation());
		return true;
	}
	else if (spot->bodyPart == nullptr)
	{
		for (int i = 0; i < bodyPart->Tags.Num(); ++i)
		{
			if (this->ActorHasTag(bodyPart->Tags[i]))
			{
				spot->bodyPart = bodyPart;

				// snap the dropped object to the component
				bodyPart->SetActorRotation(FRotator(0, 0, 0), ETeleportType::ResetPhysics);
				bodyPart->AttachToComponent(spot->tableComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
				bodyPart->SetActorLocation(spot->tableComponent->GetComponentLocation());
				return true;
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
	for (int i = 0; i < bodyPartsOnTable.Num(); ++i)
	{
		// remove the body part from the list
		if (objectToRemove == bodyPartsOnTable[i].object)
		{
			bodyPartsOnTable.RemoveAt(i);
			return true;
		}
	}

	return false;
}

bool AAssemblingTable::RemoveFromTableV2(ABodyPart* bodyPart)
{
	for (int i = 0; i < assemblingSpots.Num(); ++i)
	{
		if (assemblingSpots[i]->bodyPart == bodyPart)
		{
			bodyPart->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
			assemblingSpots[i]->bodyPart = nullptr;
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

void AAssemblingTable::Assemble(ABodyPart* bodyPart)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Success!")));
}

bool AAssemblingTable::Animate()
{
	if (bodyPartsOnTable.Num() < MinBodyParts || TemporarySpawnBody == nullptr)
	{
		return false;
	}

	// destroy each body part
	for (int i = 0; i < bodyPartsOnTable.Num(); ++i)
	{
		bodyPartsOnTable[i].object->Destroy();
	}
	bodyPartsOnTable.Empty();

	// spawn the body
	FVector location(this->GetActorLocation() + SpawnOffset);
	FRotator rotation(this->GetActorRotation() + SpawnRotation);
	FActorSpawnParameters spawnInfo;
	GetWorld()->SpawnActor<AActor>(TemporarySpawnBody, location, rotation, spawnInfo);

	return false;
}

