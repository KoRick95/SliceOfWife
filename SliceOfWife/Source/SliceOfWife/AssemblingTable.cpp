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

	TArray<ABodyPart*> bodyParts;
	TArray<int> pointersToSet;
	bool canBeDropped = false;

	if (object->IsA(ABodyPart::StaticClass()))
	{
		ABodyPart* bodyPart = Cast<ABodyPart>(object);
		bodyParts.Add(bodyPart);

		if (bodyPart->HasMeshType(spot->BodyPartType) || bodyPart->HasMeshType(CentralBodyPartType))
		{
			TArray<EBodyPartType> bodyPartTypes = bodyPart->GetCurrentMeshTypes();

			for (int i = 0; i < bodyPartTypes.Num(); ++i)
			{
				canBeDropped = false;

				if (bodyPartTypes[i] == CentralBodyPartType && centralBodyPart == nullptr)
				{
					pointersToSet.Add(-1);
					canBeDropped = true;
				}
				else
				{
					for (int j = 0; j < assemblingSpots.Num(); ++j)
					{
						if (bodyPartTypes[i] == assemblingSpots[j]->BodyPartType && !assemblingSpots[j]->IsOccupied())
						{
							pointersToSet.Add(j);
							canBeDropped = true;
							break;
						}
					}
				}

				if (!canBeDropped)
					break;
			}
		}
	}
	else if (object->IsA(AFullBody::StaticClass()))
	{
		AFullBody* body = Cast<AFullBody>(object);
		bodyParts = body->bodyParts;

		for (int i = 0; i < bodyParts.Num(); ++i)
		{
			TArray<EBodyPartType> bodyPartTypes = bodyParts[i]->GetCurrentMeshTypes();

			if (bodyParts[i]->HasMeshType(spot->BodyPartType) || bodyParts[i]->HasMeshType(CentralBodyPartType))
			{
				for (int j = 0; j < bodyPartTypes.Num(); ++j)
				{
					canBeDropped = false;

					if (bodyPartTypes[j] == CentralBodyPartType && centralBodyPart == nullptr)
					{
						pointersToSet.Add(-1);
						canBeDropped = true;
					}
					else
					{
						for (int k = 0; k < assemblingSpots.Num(); ++k)
						{
							if (bodyPartTypes[j] == assemblingSpots[k]->BodyPartType && !assemblingSpots[k]->IsOccupied())
							{
								pointersToSet.Add(k);
								canBeDropped = true;
								break;
							}
						}
					}

					if (!canBeDropped)
						break;
				}
			}
		}
	}

	if (canBeDropped)
	{
		TArray<int> typeCounts;
		int totalTypeCount = 0;

		for (int i = 0; i < bodyParts.Num(); ++i)
		{
			typeCounts.Add(bodyParts[i]->GetCurrentMeshTypes().Num());
			totalTypeCount += typeCounts[i];
		}

		if (totalTypeCount == pointersToSet.Num())
		{
			int p = 0;

			for (int b = 0; b < bodyParts.Num(); ++b)
			{
				for (int t = 0; t < typeCounts[b]; ++t)
				{
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Set pointer #%i"), p + 1));
					if (pointersToSet[p] >= 0)
					{
						assemblingSpots[pointersToSet[p]]->bodyPart = bodyParts[b];
					}
					else
					{
						centralBodyPart = bodyParts[b];
					}
					p++;
				}
			}
		}

		if (centralBodyPart == nullptr)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Central is missing")));
		}
		
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
			TArray<EBodyPartType> currentTypes = finalBody->bodyParts[i]->GetCurrentMeshTypes();

			if (finalBody->bodyParts[i] == centralBodyPart)
			{
				centralBodyPart = nullptr;
			}
			else
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
		}

		finalBody = nullptr;
		canBeRemoved = true;
	}
	else if (object->IsA(ABodyPart::StaticClass()))
	{
		ABodyPart* bodyPart = Cast<ABodyPart>(object);

		if (bodyPart == centralBodyPart)
		{
			centralBodyPart = nullptr;
		}
		else
		{
			for (int i = 0; i < assemblingSpots.Num(); ++i)
			{
				if (assemblingSpots[i]->bodyPart == bodyPart)
				{
					assemblingSpots[i]->bodyPart = nullptr;
					break;
				}
			}
		}

		canBeRemoved = true;
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
		emptyBody->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
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
