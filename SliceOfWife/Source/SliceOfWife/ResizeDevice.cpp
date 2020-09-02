// Fill out your copyright notice in the Description page of Project Settings.

#include "EnumsStructs.h"
#include "ResizeDevice.h"
#include "BodyPart.h"
#include "Engine.h"
#include "Engine/World.h"

// Sets default values
AResizeDevice::AResizeDevice()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AResizeDevice::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AResizeDevice::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool AResizeDevice::DropToDevice(AActor* object)
{
	if (object == nullptr)
		return false;

	FVector offset;

	if (object->IsA(ABodyPart::StaticClass()))
	{
		offset = SnapLocation - Cast<ABodyPart>(object)->GetMeshRelativeLocation();
	}
	else
	{
		return false;
	}

	object->AttachToActor(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	object->SetActorRelativeLocation(offset, false, nullptr, ETeleportType::ResetPhysics);
	object->SetActorRelativeRotation(FQuat(SnapRotation), false, nullptr, ETeleportType::ResetPhysics);

	objectOnDevice = object;
	return true;
}

bool AResizeDevice::RemoveFromDevice()
{
	if (objectOnDevice != nullptr)
		return false;

	objectOnDevice->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	objectOnDevice = nullptr;
	return true;
}

bool AResizeDevice::ReplaceObject(AActor* object)
{
	for (int i = 0; i < ObjectReplacements.Num(); ++i)
	{
		if (ObjectReplacements[i].Input.Get() == object->GetClass())
		{
			UClass* uClass = ObjectReplacements[i].Output.Get();
			FTransform transform = object->GetActorTransform();
			GetWorld()->SpawnActor(uClass, &transform);
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Replaced body woo!")));
			return true;
		}
	}

	return false;
}

