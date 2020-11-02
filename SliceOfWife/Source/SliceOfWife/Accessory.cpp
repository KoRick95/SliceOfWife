// Fill out your copyright notice in the Description page of Project Settings.

#include "Accessory.h"
#include "BodyPart.h"
#include "Components/MeshComponent.h"
#include "Components/PrimitiveComponent.h"

// Sets default values
AAccessory::AAccessory()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AAccessory::BeginPlay()
{
	Super::BeginPlay();

	MeshComponent = Cast<UMeshComponent>(GetComponentByClass(UMeshComponent::StaticClass()));
}

// Called every frame
void AAccessory::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AAccessory::SetPhysicsState(bool state)
{
	UPrimitiveComponent* physicsComponent = Cast<UPrimitiveComponent>(GetComponentByClass(UPrimitiveComponent::StaticClass()));

	if (physicsComponent)
	{
		physicsComponent->SetSimulatePhysics(state);
	}
}

void AAccessory::AttachToBodyPart(ABodyPart* bodyPart)
{
	if (bodyPart && this->GetClass() == bodyPart->AccessoryBlueprint.Get())
	{
		SetPhysicsState(false);
		AttachToComponent(MeshComponent, FAttachmentTransformRules::SnapToTargetIncludingScale, "Accessory_Socket");
	}
}
