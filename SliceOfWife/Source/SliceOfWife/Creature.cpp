// Fill out your copyright notice in the Description page of Project Settings.

#include "Creature.h"
#include "BodyPart.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine.h"
#include "Engine/World.h"

// Sets default values
ACreature::ACreature()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ACreature::BeginPlay()
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

	for (int i = 0; i < BodyPartBlueprints.Num(); ++i)
	{
		bodyParts.Add(BodyPartBlueprints[i].GetDefaultObject());
	}

	skeletalMeshComponent = Cast<USkeletalMeshComponent>(GetComponentByClass(USkeletalMeshComponent::StaticClass()));
}

// Called every frame
void ACreature::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ACreature::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ACreature::SetClassDefaults()
{
	USceneComponent* defaultComponent = NewObject<USceneComponent>(this);
	RootComponent = defaultComponent;
}

int ACreature::GetBodyPartsCount()
{
	return bodyParts.Num();
}

void ACreature::AttachBodyPart(ABodyPart* bodyPart)
{
	// attach the body part and add it to the array
	bodyPart->AttachToBody(this);
	this->bodyParts.Add(bodyPart);
}

bool ACreature::DetachBodyPart(ABodyPart* bodyPart)
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

int ACreature::CountCreatureTypeVariation()
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
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("%i unique body parts found."), creatureTypes.Num()));
	return creatureTypes.Num();
}
