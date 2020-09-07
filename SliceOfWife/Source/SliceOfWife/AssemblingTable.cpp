#include "AssemblingTable.h"
#include "AssemblingSpot.h"
#include "BodyPart.h"
#include "FullBody.h"
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
}

// Called every frame
void AAssemblingTable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool AAssemblingTable::DropToTable(AActor* object, AAssemblingSpot* spot)
{
	if (object == nullptr || spot == nullptr)
	{
		return false;
	}

	bool canBeDropped = false;

	if (object->IsA(AFullBody::StaticClass()))
	{
		AFullBody* body = Cast<AFullBody>(object);

		if (body->CreatureType == ECreatureType::Custom)
		{
			bool allBodyPartsPlaced = true;

			for (int b = 0; b < body->bodyParts.Num(); ++b)
			{
				bool bodyPartPlaced = false;

				for (int a = 0; a < assemblingSpots.Num(); ++a)
				{
					if (assemblingSpots[a]->SetBodyPart(body->bodyParts[b]))
					{
						bodyPartPlaced = true;
						break;
					}
				}

				if (!bodyPartPlaced)
				{
					allBodyPartsPlaced = false;
					break;
				}
			}

			if (allBodyPartsPlaced)
			{
				finalBody = body;
				canBeDropped = true;
			}
		}
	}
	else if (object->IsA(ABodyPart::StaticClass()))
	{
		ABodyPart* bodyPart = Cast<ABodyPart>(object);

		if (centralBodyPart == nullptr)
		{
			if (bodyPart->HasMeshType(CentralBodyPartType, true))
			{
				centralBodyPart = bodyPart;
				canBeDropped = true;
			}
		}
		
		if (!canBeDropped && spot->bodyPart == nullptr)
		{
			if (bodyPart->HasMeshType(spot->BodyPartType, true))
			{
				spot->bodyPart = bodyPart;
				canBeDropped = true;
			}
		}
	}

	if (canBeDropped)
	{
		// snap the body part to the table
		object->AttachToActor(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		object->SetActorRelativeLocation(SnapPosition);
		object->SetActorRelativeRotation(SnapRotation);
	}

	return canBeDropped;
}

bool AAssemblingTable::RemoveFromTable(AActor* object)
{
	if (object == nullptr)
	{
		return false;
	}

	bool canBeRemoved = false;

	if (object == finalBody)
	{
		for (int i = 0; i < finalBody->bodyParts.Num(); ++i)
		{
			for (int j = 0; j < assemblingSpots.Num(); ++j)
			{
				if (finalBody->bodyParts[i] == assemblingSpots[j]->bodyPart)
				{
					assemblingSpots[j]->bodyPart = nullptr;
					break;
				}
			}
		}

		finalBody = nullptr;
		canBeRemoved = true;
	}
	else
	{
		ABodyPart* bodyPart = Cast<ABodyPart>(object);

		if (bodyPart->HasMeshType(CentralBodyPartType))
		{
			centralBodyPart = nullptr;
			canBeRemoved = true;
		}
		else
		{
			for (int i = 0; i < assemblingSpots.Num(); ++i)
			{
				if (assemblingSpots[i]->bodyPart == bodyPart)
				{
					assemblingSpots[i]->bodyPart = nullptr;
					canBeRemoved = true;
				}
			}
		}
	}

	if (canBeRemoved)
	{
		object->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	}

	return canBeRemoved;
}

bool AAssemblingTable::BeginSewing(AAssemblingSpot* spot)
{
	if (centralBodyPart == nullptr || spot->bodyPart == nullptr || spot->bodyPart->IsAttachedToBody() || MinigameWidget == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, FString::Printf(TEXT("Check one of the following:\n"
			"1. Is it missing the central body part?\n"
			"2. Is there a body part on that spot?\n"
			"3. Is the body part already sown together?\n"
			"4. Is the table missing the widget BP?")));
		return false;
	}

	CreateWidget<UMinigameWidget>(GetWorld(), MinigameWidget.Get())->StartMinigame(spot);

	return true;
}

void AAssemblingTable::AssembleBodyPart(ABodyPart* bodyPart)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Assemble Success!")));

	if (finalBody == nullptr)
	{
		AActor* emptyBody = GetWorld()->SpawnActor(AFullBody::StaticClass(), &SnapPosition, &SnapRotation);
		emptyBody->AttachToActor(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		finalBody = Cast<AFullBody>(emptyBody);
		finalBody->CreatureType = ECreatureType::Custom;
		finalBody->AttachBodyPart(centralBodyPart);
	}

	finalBody->AttachBodyPart(bodyPart);
}

bool AAssemblingTable::AnimateBody()
{
	if (finalBody == nullptr)
	{
		return false;
	}

	finalBody->SetActorRelativeLocation(SpawnOffset);
	finalBody->SetActorRelativeRotation(SpawnRotation);
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	return false;
}
