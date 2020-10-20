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
	if (object != nullptr && !IsOccupied() && IsValidObject(object))
	{
		FVector offset = SnapLocation;

		if (object->IsA(ABodyPart::StaticClass()))
		{
			ABodyPart* bodyPart = Cast<ABodyPart>(object);
			offset = offset - bodyPart->GetMeshRelativeLocation() + bodyPart->GetMeshRadius();
		}

		object->AttachToActor(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		object->SetActorRelativeLocation(offset, false, nullptr, ETeleportType::ResetPhysics);
		object->SetActorRelativeRotation(FQuat(SnapRotation), false, nullptr, ETeleportType::ResetPhysics);

		objectOnDevice = object;
		isActive = true;

		return true;
	}
	
	return false;
}

bool AResizingDevice::RemoveFromDevice(AActor* requester)
{
	if (IsOccupied())
	{
		if (ActiveTimer < WaitTime)
		{
			objectOnDevice->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		}
		else if (ActiveTimer < FailTime)
		{
			ReplaceObject();
		}
		else
		{
			UClass* uClass = FailedProduct.Get();
			FTransform transform = objectOnDevice->GetTransform();

			objectOnDevice->Destroy();
			objectOnDevice = GetWorld()->SpawnActor(uClass, &transform);
		}
		
		if (Eject(requester))
		{
			objectOnDevice = nullptr;
			isActive = false;
			ActiveTimer = 0;
			return true;
		}
	}

	return false;
}

bool AResizingDevice::ReplaceObject()
{
	if (IsOccupied())
	{
		UClass* outputClass = GetOutputClass(objectOnDevice);

		if (outputClass)
		{
			FTransform transform = objectOnDevice->GetActorTransform();
			if (objectOnDevice->IsA(ABodyPart::StaticClass()))
			{
				ABodyPart* bodyPart = Cast<ABodyPart>(objectOnDevice);
				transform.SetLocation(this->GetActorLocation() - bodyPart->GetMeshRelativeLocation() + bodyPart->GetMeshRadius());
			}
			FActorSpawnParameters spawnParams;
			spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

			AActor* newObject = GetWorld()->SpawnActor(outputClass, &transform, spawnParams);
			objectOnDevice->Destroy();
			objectOnDevice = newObject;

			FVector offset = SnapLocation;
			if (objectOnDevice->IsA(ABodyPart::StaticClass()))
			{
				offset -= Cast<ABodyPart>(objectOnDevice)->GetMeshRelativeLocation();
			}
			objectOnDevice->SetActorLocation(this->GetActorLocation() + offset, false, nullptr, ETeleportType::ResetPhysics);

			return true;
		}
	}

	return false;
}

UClass* AResizingDevice::GetOutputClass(AActor* object)
{
	if (object != nullptr)
	{
		for (int i = 0; i < ObjectReplacements.Num(); ++i)
		{
			if (ObjectReplacements[i].Input.Get() == object->GetClass())
			{
				return ObjectReplacements[i].Output.Get();
			}
		}
	}
	
	return nullptr;
}

bool AResizingDevice::Eject(AActor* towards)
{
	if (objectOnDevice != nullptr)
	{
		UPrimitiveComponent* primitiveComponent = Cast<UPrimitiveComponent>(objectOnDevice->GetComponentByClass(UPrimitiveComponent::StaticClass()));

		if (primitiveComponent != nullptr)
		{
			FVector direction = (towards == nullptr) ? FMath::VRand() : towards->GetActorLocation() - this->GetActorLocation();
			direction.Z = 0;
			direction = direction.GetSafeNormal();
			FVector axis = FVector(direction.Y, -direction.X, 0);

			direction = direction.RotateAngleAxis(ImpulseAngle, axis);

			FVector impulse = direction * ImpulseStrength;
			primitiveComponent->SetSimulatePhysics(true);
			primitiveComponent->AddImpulse(impulse);
			return true;
		}
	}

	return false;
}

bool AResizingDevice::IsOccupied()
{
	return objectOnDevice != nullptr;
}

bool AResizingDevice::IsValidObject(AActor* object)
{
	return (bool)GetOutputClass(object);
}

void AResizingDevice::UpdateTimer()
{
	if (isActive)
	{
		ActiveTimer += this->GetWorld()->GetDeltaSeconds();
	}
}
