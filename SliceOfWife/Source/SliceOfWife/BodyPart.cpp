// Fill out your copyright notice in the Description page of Project Settings.

#include "BodyPart.h"
#include "FullBody.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine.h"

// Sets default values
ABodyPart::ABodyPart()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ABodyPart::BeginPlay()
{
	Super::BeginPlay();

	attachedBody = Cast<AFullBody>(this->GetAttachParentActor());
}

// Called every frame
void ABodyPart::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABodyPart::AttachToBody(AFullBody* fullBody)
{
	// attach to the body and store its reference
	this->AttachToActor(fullBody, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	this->attachedBody = fullBody;
}

bool ABodyPart::DetachFromBody()
{
	if (this->attachedBody != nullptr)
	{
		// detach from the body and remove its reference
		this->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		this->attachedBody = nullptr;
		return true;
	}

	return false;
}