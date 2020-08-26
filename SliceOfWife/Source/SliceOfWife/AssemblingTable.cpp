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
			centralComponent = Cast<USceneComponent>(sceneComponents[i]);
		}
	}

	if (centralComponent == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("The disassembling table has no central component.")));
	}
}

// Called every frame
void AAssemblingTable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool AAssemblingTable::DropToTableV2(ABodyPart* bodyPart, AAssemblingSpot* spot)
{
	if (bodyPart == nullptr || spot == nullptr)
	{
		return false;
	}

	if (bodyPart->ActorHasTag(CentralBodyPartTag))
	{
		centralBodyPart = bodyPart;

		// snap the dropped object to the central component
		bodyPart->SetActorRotation(FRotator(0, 0, 0), ETeleportType::ResetPhysics);
		bodyPart->AttachToComponent(centralComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		bodyPart->SetActorLocation(centralComponent->GetComponentLocation());
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

bool AAssemblingTable::RemoveFromTableV2(ABodyPart* bodyPart)
{
	if (bodyPart == nullptr)
	{
		return false;
	}

	if (bodyPart->ActorHasTag(CentralBodyPartTag))
	{
		bodyPart->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		centralBodyPart = nullptr;
		return true;
	}

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
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Assemble Success!")));
}

bool AAssemblingTable::Animate()
{
	int bodyPartsCount = 0;

	for (int i = 0; i < assemblingSpots.Num(); ++i)
	{
		if (assemblingSpots[i]->bodyPart != nullptr)
		{
			bodyPartsCount++;
		}
	}

	if (bodyPartsCount < MinBodyParts || TemporarySpawnBody == nullptr)
	{
		return false;
	}

	// destroy each body part
	for (int i = 0; i < assemblingSpots.Num(); ++i)
	{
		assemblingSpots[i]->bodyPart->Destroy();
		assemblingSpots[i]->bodyPart = nullptr;
	}

	// spawn the body
	FVector location(this->GetActorLocation() + SpawnOffset);
	FRotator rotation(this->GetActorRotation() + SpawnRotation);
	FActorSpawnParameters spawnInfo;
	GetWorld()->SpawnActor<AActor>(TemporarySpawnBody, location, rotation, spawnInfo);

	return false;
}

