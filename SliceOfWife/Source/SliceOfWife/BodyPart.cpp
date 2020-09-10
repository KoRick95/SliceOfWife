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

	if (this->GetAttachParentActor() != nullptr)
	{
		if (this->GetAttachParentActor()->IsA(AFullBody::StaticClass()))
		{
			attachedBody = Cast<AFullBody>(this->GetAttachParentActor());
			OriginalCreatureType = attachedBody->CreatureType;
		}
	}

	skeletalMeshComponent = Cast<USkeletalMeshComponent>(GetComponentByClass(USkeletalMeshComponent::StaticClass()));
	
	if (skeletalMeshComponent != nullptr)
	{
		if (BodyPartMeshes.Num() > 0)
		{
			USkeletalMesh* defaultMesh = BodyPartMeshes[0].SkeletalMesh;

			if (defaultMesh != nullptr)
			{
				skeletalMeshComponent->SetSkeletalMesh(defaultMesh);
				currentMesh = &BodyPartMeshes[0];
			}
		}
	}
}

// Called every frame
void ABodyPart::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

float ABodyPart::GetMeshRadius()
{
	return skeletalMeshComponent->SkeletalMesh->GetBounds().SphereRadius;
}

FVector ABodyPart::GetMeshRelativeLocation()
{
	return skeletalMeshComponent->SkeletalMesh->GetBounds().Origin;
}

EBodyPartType ABodyPart::GetBodyPartType()
{
	return currentMesh->BodyPartType;
}

void ABodyPart::SetPhysicsState(bool state)
{
	Cast<UPrimitiveComponent>(GetComponentByClass(UPrimitiveComponent::StaticClass()))->SetSimulatePhysics(state);
}

bool ABodyPart::IsAttachedToBody()
{
	return attachedBody != nullptr;
}

bool ABodyPart::HasMeshType(EBodyPartType type, bool switchMesh)
{
	// check from the available mesh types
	for (int i = 0; i < BodyPartMeshes.Num(); ++i)
	{
		// if a matching mesh type is found
		if (BodyPartMeshes[i].BodyPartType == type)
		{
			if (switchMesh)
			{
				// set that mesh type as current
				currentMesh = &BodyPartMeshes[i];

				// change this object's skeletal mesh
				skeletalMeshComponent->SetSkeletalMesh(currentMesh->SkeletalMesh);
			}
			
			return true;
		}
	}

	return false;
}

bool ABodyPart::AttachToBody(AFullBody* fullBody)
{
	if (fullBody != nullptr)
	{
		// attach to the body and store its reference
		this->AttachToActor(fullBody, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		this->attachedBody = fullBody;
		return true;
	}
	
	return false;
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