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

	fullBody = Cast<AFullBody>(this->GetAttachParentActor());
}

// Called every frame
void ABodyPart::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABodyPart::Attach(AFullBody* body)
{
	this->AttachToActor(body, FAttachmentTransformRules::SnapToTargetNotIncludingScale);

	fullBody = body;
}

void ABodyPart::Detach()
{
	if (fullBody == nullptr)
	{
		return;
	}

	fullBody = nullptr;
}