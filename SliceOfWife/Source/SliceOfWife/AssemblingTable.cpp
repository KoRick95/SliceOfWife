#include "AssemblingTable.h"
#include "AssemblingSpot.h"
#include "Accessory.h"
#include "BodyPart.h"
#include "Creature.h"
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
	TArray<int> spotIndexes;
	bool canBeDropped = false;

	if (object->IsA(ABodyPart::StaticClass()))
	{
		ABodyPart* bodyPart = Cast<ABodyPart>(object);
		bodyParts.Add(bodyPart);

		if (bodyPart->HasMeshOfType(CentreBodyPartType, true) || bodyPart->HasMeshOfType(spot->BodyPartType, true))
		{
			canBeDropped = CheckValidBodyPart(bodyPart, &spotIndexes);

			if (!canBeDropped)
				return false;
		}
	}
	else if (object->IsA(ACreature::StaticClass()))
	{
		ACreature* body = Cast<ACreature>(object);

		if (body->CreatureType == ECreatureType::Custom)
		{
			bodyParts = body->bodyParts;

			for (int i = 0; i < bodyParts.Num(); ++i)
			{
				canBeDropped = CheckValidBodyPart(bodyParts[i], &spotIndexes);

				if (!canBeDropped)
					return false;
			}

			if (canBeDropped)
				FinalBody = body;
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

		if (totalTypeCount == spotIndexes.Num())
		{
			int s = 0;

			for (int b = 0; b < bodyParts.Num(); ++b)
			{
				for (int t = 0; t < typeCounts[b]; ++t)
				{
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Set pointer #%i"), s + 1));
					if (spotIndexes[s] >= 0)
					{
						assemblingSpots[spotIndexes[s]]->attachedBodyPart = bodyParts[b];
					}
					else
					{
						centreBodyPart = bodyParts[b];
					}
					s++;
				}
			}

			// snap the body part to the table
			object->AttachToActor(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
			object->SetActorRelativeLocation(SnapPosition);
			object->SetActorRelativeRotation(SnapRotation);
		}
	}

	return canBeDropped;
}

bool AAssemblingTable::CanDropToTable(AActor* object, AAssemblingSpot* spot)
{
	if (object == nullptr)
	{
		return false;
	}

	TArray<ABodyPart*> bodyParts;
	bool canBeDropped = false;

	if (object->IsA(ABodyPart::StaticClass()))
	{
		ABodyPart* bodyPart = Cast<ABodyPart>(object);

		if (spot == nullptr)
		{
			canBeDropped = bodyPart->HasMeshOfType(CentreBodyPartType, true);
		}
		else
		{
			canBeDropped = bodyPart->HasMeshOfType(CentreBodyPartType, true) || bodyPart->HasMeshOfType(spot->BodyPartType, true);
		}

		if (canBeDropped)
		{
			canBeDropped = CheckValidBodyPart(bodyPart);
		}
	}
	else if (object->IsA(ACreature::StaticClass()))
	{
		ACreature* body = Cast<ACreature>(object);

		if (body->CreatureType == ECreatureType::Custom)
		{
			bodyParts = body->bodyParts;

			for (int i = 0; i < bodyParts.Num(); ++i)
			{
				canBeDropped = CheckValidBodyPart(bodyParts[i]);

				if (!canBeDropped)
					break;
			}
		}
	}
	else if (object->IsA(AAccessory::StaticClass()))
	{
		if (spot->attachedBodyPart)
		{
			canBeDropped = Cast<AAccessory>(object)->AttachToBodyPart(spot->attachedBodyPart);
		}
	}

	return canBeDropped;
}

bool AAssemblingTable::CheckValidBodyPart(ABodyPart* bodyPart, TArray<int>* spotIndexes)
{
	bool isValid = false;
	TArray<EBodyPartType> bodyPartTypes = bodyPart->GetCurrentMeshTypes();

	// check all body part types
	for (int i = 0; i < bodyPartTypes.Num(); ++i)
	{
		isValid = false;

		// if the body part is the centre and the centre spot is unoccupied
		if (bodyPartTypes[i] == CentreBodyPartType && centreBodyPart == nullptr)
		{
			// if a spot index list is passed as paramater, fill the list
			if (spotIndexes != nullptr)
			{
				spotIndexes->Add(-1);
			}
			
			isValid = true;
		}
		else
		{
			// check all the assembling spots
			for (int j = 0; j < assemblingSpots.Num(); ++j)
			{
				// if the body part type match the assembling spot's assigned type AND the assembling spot is occupied
				if (bodyPartTypes[i] == assemblingSpots[j]->BodyPartType && !assemblingSpots[j]->IsOccupied())
				{
					// if a spot index list is passed as paramater, fill the list
					if (spotIndexes != nullptr)
					{
						spotIndexes->Add(j);
					}
					
					isValid = true;
					break;
				}
			}
		}

		// if one of the body part is not valid, then exit the loop
		if (!isValid)
			break;
	}

	return isValid;
}

bool AAssemblingTable::RemoveFromTable(AActor* object)
{
	if (object == nullptr)
	{
		return false;
	}

	bool canBeRemoved = false;

	if (object == FinalBody)
	{
		for (int i = 0; i < FinalBody->bodyParts.Num(); ++i)
		{
			if (FinalBody->bodyParts[i] == centreBodyPart)
			{
				centreBodyPart = nullptr;
			}
			else
			{
				for (int j = 0; j < assemblingSpots.Num(); ++j)
				{
					if (FinalBody->bodyParts[i] == assemblingSpots[j]->attachedBodyPart)
					{
						assemblingSpots[j]->attachedBodyPart = nullptr;
						break;
					}
				}
			}
		}

		FinalBody = nullptr;
		canBeRemoved = true;
	}
	else if (object->IsA(ABodyPart::StaticClass()))
	{
		ABodyPart* bodyPart = Cast<ABodyPart>(object);

		if (bodyPart == centreBodyPart)
		{
			centreBodyPart = nullptr;
		}

		for (int i = 0; i < assemblingSpots.Num(); ++i)
		{
			if (assemblingSpots[i]->attachedBodyPart == bodyPart)
			{
				assemblingSpots[i]->attachedBodyPart = nullptr;
			}
		}

		bodyPart->SwitchMesh();
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
	if (centreBodyPart == nullptr || spot->attachedBodyPart == nullptr || spot->attachedBodyPart->IsAttachedToBody() || MinigameWidget == nullptr)
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

	if (FinalBody == nullptr)
	{
		AActor* emptyBody = GetWorld()->SpawnActor(ACreature::StaticClass());
		FinalBody = Cast<ACreature>(emptyBody);
		FinalBody->SetClassDefaults();
		FinalBody->AttachToActor(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		FinalBody->SetActorRelativeLocation(SnapPosition);
		FinalBody->SetActorRelativeRotation(SnapRotation);
		FinalBody->CreatureType = ECreatureType::Custom;
		FinalBody->AttachBodyPart(centreBodyPart);
	}

	FinalBody->AttachBodyPart(bodyPart);
}

void AAssemblingTable::AssembleAllBodyParts()
{
	for (int i = 0; i < assemblingSpots.Num(); ++i)
	{
		ABodyPart* bodyPart = assemblingSpots[i]->attachedBodyPart;

		if (bodyPart && !bodyPart->IsAttachedToBody())
		{
			AssembleBodyPart(bodyPart);
		}
	}
}

bool AAssemblingTable::AnimateBody()
{
	if (FinalBody)
	{
		FinalBody->SetActorRelativeLocation(SpawnOffset);
		FinalBody->SetActorRelativeRotation(SpawnRotation);
		DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		
		return true;
	}

	return false;
}

bool AAssemblingTable::IsCentreOccupied()
{
	return centreBodyPart != nullptr;
}

bool AAssemblingTable::IsCentreBodyPart(ABodyPart* bodyPart)
{
	if (bodyPart && bodyPart->HasMeshOfType(CentreBodyPartType))
		return true;

	return false;
}

int AAssemblingTable::GetUnassembledSpotsCount()
{
	int count = 0;

	for (int i = 0; i < assemblingSpots.Num(); ++i)
	{
		ABodyPart* bodyPart = assemblingSpots[i]->attachedBodyPart;

		if (bodyPart && !bodyPart->IsAttachedToBody())
		{
			count++;
		}
	}

	return count;
}
