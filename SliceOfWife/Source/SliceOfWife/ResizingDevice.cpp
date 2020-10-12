// Fill out your copyright notice in the Description page of Project Settings.

#include "ResizingDevice.h"
#include "BodyPart.h"
#include "Engine.h"
#include "Engine/World.h"

// Sets default values
AResizingDevice::AResizingDevice()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AResizingDevice::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AResizingDevice::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool AResizingDevice::DropToDevice(AActor* object)
{
	if (object != nullptr && !IsOccupied())
	{
		FVector offset = SnapLocation;

		if (object->IsA(ABodyPart::StaticClass()))
		{
			offset -= Cast<ABodyPart>(object)->GetMeshRelativeLocation();
			object->AttachToActor(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
			object->SetActorRelativeLocation(offset, false, nullptr, ETeleportType::ResetPhysics);
			object->SetActorRelativeRotation(FQuat(SnapRotation), false, nullptr, ETeleportType::ResetPhysics);

			objectOnDevice = object;

			return true;
		}
	}
	
	return false;
}

bool AResizingDevice::RemoveFromDevice()
{
	if (!IsOccupied())
		return false;

	objectOnDevice->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	objectOnDevice = nullptr;
	return true;
}

bool AResizingDevice::ReplaceObject()
{
	if (!IsOccupied())
		return false;

	for (int i = 0; i < ObjectReplacements.Num(); ++i)
	{
		if (ObjectReplacements[i].Input.Get() == objectOnDevice->GetClass())
		{
			UClass* uClass = ObjectReplacements[i].Output.Get();
			FTransform transform = objectOnDevice->GetActorTransform();
			AActor* newObject = GetWorld()->SpawnActor(uClass, &transform);
			AActor* oldObject = objectOnDevice;
			objectOnDevice = newObject;
			oldObject->Destroy();
			return true;
		}
	}

	return false;
}

bool AResizingDevice::IsOccupied()
{
	return objectOnDevice != nullptr;
}