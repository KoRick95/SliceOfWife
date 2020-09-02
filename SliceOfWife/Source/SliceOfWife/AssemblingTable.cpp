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

bool AAssemblingTable::DropToTable(ABodyPart* bodyPart, AAssemblingSpot* spot)
{
	if (bodyPart == nullptr || spot == nullptr)
	{
		return false;
	}

	bool dropped = false;

	if (bodyPart->IsOfType(CentralBodyPartType))
	{
		centralBodyPart = bodyPart;

		// snap
		bodyPart->AttachToActor(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		bodyPart->SetActorRelativeLocation(SnapPosition);
		bodyPart->SetActorRelativeRotation(SnapRotation);

		dropped = true;
	}
	// if spot is not already occupied by another body part
	else if (spot->bodyPart == nullptr)
	{
		if (bodyPart->IsOfType(spot->BodyPartType))
		{
			spot->bodyPart = bodyPart;

			// snap
			bodyPart->AttachToActor(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
			bodyPart->SetActorRelativeLocation(SnapPosition);
			bodyPart->SetActorRelativeRotation(SnapRotation);

			dropped = true;
		}
	}

	return dropped;
}

bool AAssemblingTable::RemoveFromTable(ABodyPart* bodyPart)
{
	if (bodyPart == nullptr)
	{
		return false;
	}

	if (bodyPart->IsOfType(CentralBodyPartType))
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

bool AAssemblingTable::BeginSewing(AAssemblingSpot* spot)
{
	if (centralBodyPart == nullptr || spot->bodyPart == nullptr || spot->bodyPart->IsAttachedToBody() || WidgetBP == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, FString::Printf(TEXT("Check one of the following:\n"
			"1. Is it missing the central body part?\n"
			"2. Is there a body part on that spot?\n"
			"3. Is the body part already sown together?\n"
			"4. Is the table missing the widget BP?")));
		return false;
	}

	widget = CreateWidget<UMinigameWidget>(GetWorld(), WidgetBP.Get());
	widget->StartMinigame(spot);

	return true;
}

void AAssemblingTable::AssembleBodyPart(ABodyPart* bodyPart)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Assemble Success!")));

	if (finalBody == nullptr)
	{
		FTransform transform;
		transform.SetLocation(SnapPosition);
		transform.SetRotation(FQuat(SnapRotation));
		AActor* emptyBody = GetWorld()->SpawnActor(AFullBody::StaticClass(), &transform);
		emptyBody->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
		finalBody = Cast<AFullBody>(emptyBody);
		finalBody->AttachBodyPart(centralBodyPart);
	}

	finalBody->AttachBodyPart(bodyPart);
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

