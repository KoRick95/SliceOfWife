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
	TArray<int> spotIndexes;
	bool canBeDropped = false;

	if (object->IsA(ABodyPart::StaticClass()))
	{
		ABodyPart* bodyPart = Cast<ABodyPart>(object);
		bodyParts.Add(bodyPart);

		if (bodyPart->HasMeshOfType(CentralBodyPartType, true) || bodyPart->HasMeshOfType(spot->BodyPartType, true))
		{
			canBeDropped = CheckBodyPart(bodyPart, &spotIndexes);

			if (!canBeDropped)
				return false;
		}
	}
	else if (object->IsA(AFullBody::StaticClass()))
	{
		AFullBody* body = Cast<AFullBody>(object);

		if (body->CreatureType == ECreatureType::Custom)
		{
			bodyParts = body->bodyParts;

			for (int i = 0; i < bodyParts.Num(); ++i)
			{
				canBeDropped = CheckBodyPart(bodyParts[i], &spotIndexes);

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
						assemblingSpots[spotIndexes[s]]->bodyPart = bodyParts[b];
					}
					else
					{
						centralBodyPart = bodyParts[b];
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

bool AAssemblingTable::CheckBodyPart(ABodyPart* bodyPart, TArray<int>* spotIndexes)
{
	bool isValid = false;
	TArray<EBodyPartType> bodyPartTypes = bodyPart->GetCurrentMeshTypes();

	for (int i = 0; i < bodyPartTypes.Num(); ++i)
	{
		isValid = false;

		if (bodyPartTypes[i] == CentralBodyPartType && centralBodyPart == nullptr)
		{
			spotIndexes->Add(-1);
			isValid = true;
		}
		else
		{
			for (int j = 0; j < assemblingSpots.Num(); ++j)
			{
				if (bodyPartTypes[i] == assemblingSpots[j]->BodyPartType && !assemblingSpots[j]->IsOccupied())
				{
					spotIndexes->Add(j);
					isValid = true;
					break;
				}
			}
		}

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
			if (FinalBody->bodyParts[i] == centralBodyPart)
			{
				centralBodyPart = nullptr;
			}
			else
			{
				for (int j = 0; j < assemblingSpots.Num(); ++j)
				{
					if (FinalBody->bodyParts[i] == assemblingSpots[j]->bodyPart)
					{
						assemblingSpots[j]->bodyPart = nullptr;
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

	if (FinalBody == nullptr)
	{
		AActor* emptyBody = GetWorld()->SpawnActor(AFullBody::StaticClass(), &SnapPosition, &SnapRotation);
		emptyBody->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
		FinalBody = Cast<AFullBody>(emptyBody);
		FinalBody->CreatureType = ECreatureType::Custom;
		FinalBody->AttachBodyPart(centralBodyPart);
	}

	FinalBody->AttachBodyPart(bodyPart);
}

bool AAssemblingTable::AnimateBody()
{
	if (FinalBody == nullptr)
	{
		return false;
	}

	FinalBody->SetActorRelativeLocation(SpawnOffset);
	FinalBody->SetActorRelativeRotation(SpawnRotation);
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	return false;
}
