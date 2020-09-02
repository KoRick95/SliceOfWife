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

	USkeletalMeshComponent* skeletalMeshComponent = Cast<USkeletalMeshComponent>(GetComponentByClass(USkeletalMeshComponent::StaticClass()));
	
	if (skeletalMeshComponent != nullptr)
	{
		skeletalMesh = skeletalMeshComponent->SkeletalMesh;
	}

	if (BodyPartTypes.Num() == 0)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("A body part is not assigned a tag.")));
	}
}

// Called every frame
void ABodyPart::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

FVector ABodyPart::GetMeshRelativeLocation()
{
	return skeletalMesh->GetBounds().Origin;
}

float ABodyPart::GetMeshRadius()
{
	return skeletalMesh->GetBounds().SphereRadius;
}

bool ABodyPart::IsAttachedToBody()
{
	return attachedBody != nullptr;
}

bool ABodyPart::IsOfType(EBodyPartType type)
{
	for (int i = 0; i < BodyPartTypes.Num(); ++i)
	{ 
		if (BodyPartTypes[i] == type)
		{
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