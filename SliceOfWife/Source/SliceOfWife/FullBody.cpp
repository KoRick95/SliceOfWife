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

	// add any 
	for (int i = 0; i < children.Num(); ++i)
	{
		bodyParts.Add(Cast<ABodyPart>(children[i]));
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

void AFullBody::Attach(ABodyPart* bodyPart)
{
	// attach the body part and add it to the array
	bodyPart->AttachTo(this);
	bodyParts.Add(bodyPart);
}

bool AFullBody::Detach(ABodyPart* bodyPart)
{
	// check if the body part is a part of this body
	for (int i = 0; i < bodyParts.Num(); ++i)
	{
		if (bodyPart == bodyParts[i])
		{
			// detach the body part and remove it from the array
			bodyPart->Detach();
			bodyParts.RemoveAt(i);
			return true;
		}
	}

	return false;
}
