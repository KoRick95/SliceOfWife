// Fill out your copyright notice in the Description page of Project Settings.

#include "BodyPart.h"
#include "Creature.h"
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
		if (this->GetAttachParentActor()->IsA(ACreature::StaticClass()))
		{
			attachedBody = Cast<ACreature>(this->GetAttachParentActor());
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
		else if (skeletalMeshComponent->SkeletalMesh != nullptr)
		{
			FBodyPartMesh newBodyPartMesh;
			newBodyPartMesh.SkeletalMesh = skeletalMeshComponent->SkeletalMesh;
			newBodyPartMesh.BodyPartTypes.Add(EBodyPartType::Other);
			currentMesh = &newBodyPartMesh;

			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, FString::Printf(TEXT("A body part is not assigned a type.")));
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

TArray<EBodyPartType> ABodyPart::GetCurrentMeshTypes()
{
	TArray<EBodyPartType> currentMeshTypes;

	for (int i = 0; i < currentMesh->BodyPartTypes.Num(); ++i)
	{
		currentMeshTypes.Add(currentMesh->BodyPartTypes[i]);
	}

	return currentMeshTypes;
}

void ABodyPart::SetPhysicsState(bool state)
{
	Cast<UPrimitiveComponent>(GetComponentByClass(UPrimitiveComponent::StaticClass()))->SetSimulatePhysics(state);
}

bool ABodyPart::IsAttachedToBody()
{
	return attachedBody != nullptr;
}

bool ABodyPart::HasMeshOfType(EBodyPartType type, bool switchMesh)
{
	// check from the available mesh types
	for (int i = 0; i < BodyPartMeshes.Num(); ++i)
	{
		for (int j = 0; j < BodyPartMeshes[i].BodyPartTypes.Num(); ++j)
		{
			// if a matching mesh type is found
			if (BodyPartMeshes[i].BodyPartTypes[j] == type)
			{
				if (switchMesh)
				{
					SwitchMesh(i);
				}

				return true;
			}
		}
	}

	return false;
}

bool ABodyPart::SwitchMesh(int index)
{
	if (index >= 0 && index < BodyPartMeshes.Num())
	{
		// set that mesh type as current
		currentMesh = &BodyPartMeshes[index];

		// change this object's skeletal mesh
		skeletalMeshComponent->SetSkeletalMesh(currentMesh->SkeletalMesh);

		return true;
	}

	return false;
}

bool ABodyPart::AttachToBody(ACreature* fullBody)
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