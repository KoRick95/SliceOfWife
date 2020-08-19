// Fill out your copyright notice in the Description page of Project Settings.

#include "BodyPart.h"
#include "FullBody.h"
#include "Components/SkeletalMeshComponent.h"

// Sets default values
ABodyPart::ABodyPart()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Tags.Add("Pickup");
	Tags.Add("BodyPart");
}

// Called when the game starts or when spawned
void ABodyPart::BeginPlay()
{
	Super::BeginPlay();

	body = Cast<AFullBody>(this->GetAttachParentActor());
}

// Called every frame
void ABodyPart::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABodyPart::AttachTo(AFullBody* body)
{
	// attach to the body and store its reference
	this->AttachToActor(body, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	body = body;
}

bool ABodyPart::Detach()
{
	if (body != nullptr)
	{
		// detach from the body and remove its reference
		this->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		body = nullptr;
		return true;
	}

	return false;
}