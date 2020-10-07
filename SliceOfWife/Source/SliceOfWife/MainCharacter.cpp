// Fill out your copyright notice in the Description page of Project Settings.
#include "MainCharacter.h"
#include "AnimatingDevice.h"
#include "AssemblingTable.h"
#include "AssemblingSpot.h"
#include "BodyPart.h"
#include "BodyStorage.h"
#include "Creature.h"
#include "DisassemblingTable.h"
#include "ResizingDevice.h"
#include "Soul.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/SceneComponent.h"
#include "Components/SphereComponent.h"
#include "Engine.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"

// Sets default values
AMainCharacter::AMainCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationRoll = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
}

// Called when the game starts or when spawned
void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();

	springArm = Cast<USpringArmComponent>(this->GetComponentByClass(USpringArmComponent::StaticClass()));
	camera = Cast<UCameraComponent>(this->GetComponentByClass(UCameraComponent::StaticClass()));

	if (springArm == nullptr || camera == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Player camera is not set up!")));
	}

	springArm->bUsePawnControlRotation = false;
	springArm->SetAbsolute(false, true, false);
}

// Called every frame
void AMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMainCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMainCharacter::MoveRight);

	PlayerInputComponent->BindAxis("LookUp", this, &AMainCharacter::LookUp);
	PlayerInputComponent->BindAxis("LookRight", this, &AMainCharacter::LookRight);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("PickUp", IE_Pressed, this, &AMainCharacter::PickUpAndDrop);
	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &AMainCharacter::Interact);
}

void AMainCharacter::MoveForward(float axis)
{
	if (axis)
	{
		FVector direction = camera->GetForwardVector().GetSafeNormal() * MoveSpeed;
		direction.Z = 0;
		FRotator rotation = FVector(camera->GetForwardVector() * axis).ToOrientationRotator();
		rotation.Roll = 0;
		rotation.Pitch = 0;
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, FString::Printf(TEXT("direction: %f, %f, %f"), direction.X, direction.Y, direction.Z));
		this->AddMovementInput(direction, axis);
		this->SetActorRotation(rotation);
	}
}

void AMainCharacter::MoveRight(float axis)
{
	if (axis)
	{
		FVector direction = camera->GetRightVector() * MoveSpeed;
		direction.Z = 0;
		FRotator rotation = FVector(camera->GetRightVector() * axis).ToOrientationRotator();
		rotation.Roll = 0;
		rotation.Pitch = 0;

		this->AddMovementInput(direction, axis);
		this->SetActorRotation(rotation);
	}
}

void AMainCharacter::LookUp(float axis)
{
	float pitch = springArm->GetComponentRotation().Pitch + axis * CameraSensitivity;

	if (pitch > CameraVerticalMin && pitch < CameraVerticalMax)
	{
		springArm->AddLocalRotation(FRotator(axis, 0, 0) * CameraSensitivity);
	}
}

void AMainCharacter::LookRight(float axis)
{
	springArm->AddWorldRotation(FRotator(0, axis, 0) * CameraSensitivity);
}

void AMainCharacter::PickUpAndDrop()
{
	// get all nearby objects
	TArray<AActor*> nearbyObjects;
	this->GetOverlappingActors(nearbyObjects);

	// if the player is not holding anything
	if (heldObject == nullptr)
	{
		// check all nearby objects
		for (int i = 0; i < nearbyObjects.Num(); ++i)
		{
			AActor* objectToHold = nullptr;

			if (nearbyObjects[i]->IsA(ABodyStorage::StaticClass()))
			{
				objectToHold = Cast<ABodyStorage>(nearbyObjects[i])->TakeBody();
			}
			else if (nearbyObjects[i]->IsA(AResizingDevice::StaticClass()))
			{
				objectToHold = Cast<AResizingDevice>(nearbyObjects[i])->objectOnDevice;
			}
			else if (nearbyObjects[i]->IsA(ACreature::StaticClass()))
			{
				objectToHold = nearbyObjects[i];
			}
			else if (nearbyObjects[i]->IsA(ABodyPart::StaticClass()))
			{
				ABodyPart* aBodyPart = Cast<ABodyPart>(nearbyObjects[i]);

				// if the body part is attached to a body
				if (aBodyPart->attachedBody != nullptr)
				{
					objectToHold = aBodyPart->attachedBody;
				}
				else
				{
					objectToHold = aBodyPart;
				}
			}

			if (objectToHold != nullptr)
			{
				// get the object's attach parent
				AActor* objectAttachParent = objectToHold->GetAttachParentActor();

				bool canPickupObject = true;

				// if the object has an attach parent
				if (objectAttachParent != nullptr)
				{
					if (objectAttachParent->IsA(AAssemblingTable::StaticClass()))
					{
						// remove it from the assembling table
						canPickupObject = Cast<AAssemblingTable>(objectAttachParent)->RemoveFromTable(objectToHold);
					}
					else if (objectAttachParent->IsA(ADisassemblingTable::StaticClass()))
					{
						// remove it from the disassembling table
						canPickupObject = Cast<ADisassemblingTable>(objectAttachParent)->RemoveFromTable();
					}
					else if (objectAttachParent->IsA(AResizingDevice::StaticClass()))
					{
						// remove it from the resize device
						canPickupObject = Cast<AResizingDevice>(objectAttachParent)->RemoveFromDevice();
					}
					else
					{
						canPickupObject = false;
					}
				}

				if (canPickupObject)
				{
					// get player to hold the object
					HoldObject(objectToHold);
					break;
				}
			}
		}
	}
	else
	{
		bool isSnapped = false;

		// detach the held object from the player
		heldObject->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

		// check nearby objects
		for (int i = 0; i < nearbyObjects.Num(); ++i)
		{
			if (nearbyObjects[i]->IsA(AAssemblingSpot::StaticClass()))
			{
				isSnapped = Cast<AAssemblingSpot>(nearbyObjects[i])->DropToTable(heldObject);
			}
			else if (nearbyObjects[i]->IsA(ADisassemblingTable::StaticClass()))
			{
				isSnapped = Cast<ADisassemblingTable>(nearbyObjects[i])->DropToTable(heldObject);
			}
			else if (nearbyObjects[i]->IsA(AResizingDevice::StaticClass()))
			{
				isSnapped = Cast<AResizingDevice>(nearbyObjects[i])->DropToDevice(heldObject);
			}

			if (isSnapped)
				break;
		}

		if (!isSnapped)
		{
			// enable physics
			TArray<UActorComponent*> components = heldObject->GetComponentsByClass(UPrimitiveComponent::StaticClass());
			for (int i = 0; i < components.Num(); ++i)
			{
				Cast<UPrimitiveComponent>(components[i])->SetSimulatePhysics(true);
			}
		}

		heldObject = nullptr;
	}
}

bool AMainCharacter::HoldObject(AActor* objectToHold)
{
	if (objectToHold == nullptr)
	{
		return false;
	}

	// disable physics on the object
	TArray<UActorComponent*> primitiveComponents = objectToHold->GetComponentsByClass(UPrimitiveComponent::StaticClass());
	for (int i = 0; i < primitiveComponents.Num(); ++i)
	{
		Cast<UPrimitiveComponent>(primitiveComponents[i])->SetSimulatePhysics(false);
	}

	if (objectToHold->IsA(ACreature::StaticClass()))
	{
		ACreature* fullBody = Cast<ACreature>(objectToHold);

		// attach the object to the player
		fullBody->AttachToActor(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale);

		// add offset to the object
		fullBody->SetActorRelativeLocation(PickupOffset, false, nullptr, ETeleportType::ResetPhysics);
	}
	else if (objectToHold->IsA(ABodyPart::StaticClass()))
	{
		// get the object's skeletal mesh component
		ABodyPart* bodyPart = Cast<ABodyPart>(objectToHold);

		// calculate the mesh offset
		FVector meshCentre = bodyPart->GetMeshRelativeLocation();
		float meshHalfHeight = bodyPart->GetMeshRadius();
		FVector meshOffset = FVector(0, 0, meshHalfHeight) - meshCentre;

		// attach the object to the player
		objectToHold->AttachToActor(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale);

		// add the offset to the object
		objectToHold->SetActorRelativeLocation(FVector(0) + PickupOffset, false, nullptr, ETeleportType::ResetPhysics);
		objectToHold->AddActorLocalOffset(meshOffset, false, nullptr, ETeleportType::ResetPhysics);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Invalid object type.")));
		return false;
	}

	heldObject = objectToHold;
	return true;
}

void AMainCharacter::Interact()
{
	TArray<AActor*> nearbyObjects;
	this->GetOverlappingActors(nearbyObjects);

	bool hasInteracted = false;

	for (int i = 0; i < nearbyObjects.Num(); ++i)
	{
		if (nearbyObjects[i]->IsA(ASoul::StaticClass()))
		{
			Cast<ASoul>(nearbyObjects[i])->Despawn();
			hasInteracted = true;
		}
		else if (nearbyObjects[i]->IsA(ADisassemblingTable::StaticClass()))
		{
			hasInteracted = Cast<ADisassemblingTable>(nearbyObjects[i])->Charge();
		}
		else if (nearbyObjects[i]->IsA(AAssemblingSpot::StaticClass()))
		{
			hasInteracted = Cast<AAssemblingSpot>(nearbyObjects[i])->BeginSewing();
		}
		else if (nearbyObjects[i]->IsA(AResizingDevice::StaticClass()))
		{
			hasInteracted = Cast<AResizingDevice>(nearbyObjects[i])->ReplaceObject();
		}
		else if (nearbyObjects[i]->IsA(AAnimatingDevice::StaticClass()))
		{
			hasInteracted = Cast<AAnimatingDevice>(nearbyObjects[i])->AnimateBody();
		}

		if (hasInteracted)
			break;
	}
}

void AMainCharacter::OnOverlapBegin(UPrimitiveComponent* OverLappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Fucked.")));
}
