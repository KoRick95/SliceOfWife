// Fill out your copyright notice in the Description page of Project Settings.

#include "FullBody.h"
#include "BodyPart.h"
#include "Engine/World.h"

// Sets default values
AFullBody::AFullBody()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void AFullBody::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> children;
	this->GetAllChildActors(children);

	// add any body part that is attached to the body
	for (int i = 0; i < children.Num(); ++i)
	{
		if (children[i]->IsA(ABodyPart::StaticClass()))
		{
			this->bodyParts.Add(Cast<ABodyPart>(children[i]));
		}
	}
}

// Called every frame
void AFullBody::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AFullBody::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

int AFullBody::GetBodyPartsCount()
{
	return bodyParts.Num();
}

void AFullBody::AttachBodyPart(ABodyPart* bodyPart)
{
	// attach the body part and add it to the array
	bodyPart->AttachToBody(this);
	this->bodyParts.Add(bodyPart);
}

bool AFullBody::DetachBodyPart(ABodyPart* bodyPart)
{
	// check if the body part is a part of this body
	for (int i = 0; i < this->bodyParts.Num(); ++i)
	{
		if (bodyPart == this->bodyParts[i])
		{
			// detach the body part and remove it from the array
			bodyPart->DetachFromBody();
			this->bodyParts.RemoveAt(i);
			return true;
		}
	}

	return false;
}

int AFullBody::CountCreatureTypeVariation()
{
	TArray<ECreatureType> creatureTypes;

	for (int b = 0; b < bodyParts.Num(); ++b)
	{
		bool unique = true;

		for (int c = 0; c < creatureTypes.Num(); ++c)
		{
			if (bodyParts[b]->OriginalCreatureType == creatureTypes[c])
			{
				unique = false;
				break;
			}
		}

		if (unique)
		{
			creatureTypes.Add(bodyParts[b]->OriginalCreatureType);
		}
	}

	return creatureTypes.Num();
}
