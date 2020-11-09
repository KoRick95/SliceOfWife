// Fill out your copyright notice in the Description page of Project Settings.
#include "MainCharacter.h"
#include "AnimatingDevice.h"
#include "AssemblingTable.h"
#include "AssemblingSpot.h"
#include "BodyPart.h"
#include "BodyStorage.h"
#include "Creature.h"
#include "DisassemblyTable.h"
#include "ResizingDevice.h"
#include "Soul.h"
#include "Camera/CameraComponent.h"
#include "Components/SceneComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Components/InputComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine.h"
#include "Engine/StaticMesh.h"
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
	if (CanMove && axis)
	{
		FVector direction = camera->GetForwardVector() * MoveSpeed;
		direction.Z = 0;
		float vecDist = FMath::Sqrt(direction.X * direction.X + direction.Y * direction.Y);
		float toScale = 1 / vecDist;
		direction *= toScale;

		FRotator rotation = FVector(camera->GetForwardVector() * axis).ToOrientationRotator();
		rotation.Roll = 0;
		rotation.Pitch = 0;

		this->AddMovementInput(direction, axis);
		this->SetActorRotation(rotation);
	}
}

void AMainCharacter::MoveRight(float axis)
{
	if (CanMove && axis)
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
	float pitch = springArm->GetComponentRotation().Pitch + axis * VerticalCameraSensitivity;

	if (pitch > CameraVerticalMin && pitch < CameraVerticalMax)
	{
		springArm->AddLocalRotation(FRotator(axis, 0, 0) * VerticalCameraSensitivity);
	}
}

void AMainCharacter::LookRight(float axis)
{
	springArm->AddWorldRotation(FRotator(0, axis, 0) * HorizontalCameraSensitivity);
}

void AMainCharacter::PickUpAndDrop()
{
	if (!CanPickUp)
		return;

	// get all nearby objects
	TArray<AActor*> nearbyObjects;
	this->GetOverlappingActors(nearbyObjects);

	// if the player is not holding anything
	if (HeldObject == nullptr)
	{
		// check all nearby objects
		for (int i = 0; i < nearbyObjects.Num(); ++i)
		{
			AActor* objectToHold = nullptr;

			if (nearbyObjects[i]->IsA(ABodyStorage::StaticClass()))
			{
				objectToHold = Cast<ABodyStorage>(nearbyObjects[i])->TakeBody();
			}
			else if (nearbyObjects[i]->IsA(ACreature::StaticClass()))
			{
				objectToHold = nearbyObjects[i];
			}
			else if (nearbyObjects[i]->IsA(ABodyPart::StaticClass()))
			{
				ABodyPart* bodyPart = Cast<ABodyPart>(nearbyObjects[i]);

				// if the body part is attached to a body
				if (bodyPart->attachedBody != nullptr)
				{
					objectToHold = bodyPart->attachedBody;
				}
				else
				{
					objectToHold = bodyPart;
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
					else if (objectAttachParent->IsA(ADisassemblyTable::StaticClass()))
					{
						// remove it from the disassembling table
						canPickupObject = Cast<ADisassemblyTable>(objectAttachParent)->RemoveFromTable();
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
		HeldObject->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

		// check nearby objects
		for (int i = 0; i < nearbyObjects.Num(); ++i)
		{
			if (nearbyObjects[i]->IsA(AAssemblingSpot::StaticClass()))
			{
				isSnapped = Cast<AAssemblingSpot>(nearbyObjects[i])->DropToTable(HeldObject);
			}
			else if (nearbyObjects[i]->IsA(ADisassemblyTable::StaticClass()))
			{
				isSnapped = Cast<ADisassemblyTable>(nearbyObjects[i])->DropToTable(HeldObject);
			}
			else if (nearbyObjects[i]->IsA(AResizingDevice::StaticClass()))
			{
				isSnapped = Cast<AResizingDevice>(nearbyObjects[i])->DropToDevice(HeldObject);
			}

			if (isSnapped)
				break;
		}

		if (!isSnapped)
		{
			// enable physics
			TArray<UActorComponent*> primitiveComponents = HeldObject->GetComponentsByClass(UPrimitiveComponent::StaticClass());
			for (int i = 0; i < primitiveComponents.Num(); ++i)
			{
				UPrimitiveComponent* physicsComponent = Cast<UPrimitiveComponent>(primitiveComponents[i]);
				physicsComponent->SetSimulatePhysics(true);

				if (physicsComponent != HeldObject->GetRootComponent())
				{
					physicsComponent->AttachToComponent(HeldObject->GetRootComponent(), FAttachmentTransformRules::SnapToTargetIncludingScale);
				}
			}
		}

		HeldObject = nullptr;
		PopBubble();
	}
}

bool AMainCharacter::HoldObject(AActor* objectToHold)
{
	if (objectToHold == nullptr)
	{
		return false;
	}

	// disable physics on the object
	TArray<UActorComponent*> physicsComponents = objectToHold->GetComponentsByClass(UPrimitiveComponent::StaticClass());
	for (int i = 0; i < physicsComponents.Num(); ++i)
	{
		UPrimitiveComponent* physicsComponent = Cast<UPrimitiveComponent>(physicsComponents[i]);
		physicsComponent->SetSimulatePhysics(false);

		if (physicsComponent != objectToHold->GetRootComponent())
		{
			physicsComponent->AttachToComponent(objectToHold->GetRootComponent(), FAttachmentTransformRules::SnapToTargetIncludingScale);
		}
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
		float meshRadius = bodyPart->GetMeshRadius();
		FVector meshOffset = FVector(0, 0, meshRadius) - meshCentre;

		// attach the object to the player
		objectToHold->AttachToActor(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale);

		// add the offset to the object
		objectToHold->SetActorRelativeLocation(PickupOffset + meshOffset, false, nullptr, ETeleportType::ResetPhysics);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Invalid object type.")));
		return false;
	}

	ApplyBubble(objectToHold);

	HeldObject = objectToHold;
	return true;
}

void AMainCharacter::ApplyBubble(AActor* Object)
{
	if (Object)
	{
		UMeshComponent* MeshComponent = Cast<UMeshComponent>(Object->GetComponentByClass(UMeshComponent::StaticClass()));
		
		if (MeshComponent)
		{
			AActor* NewBubble = GetWorld()->SpawnActor(BubbleBlueprint);
			UStaticMeshComponent* BubbleMeshComponent = Cast<UStaticMeshComponent>(NewBubble->GetComponentByClass(UStaticMeshComponent::StaticClass()));

			if (BubbleMeshComponent)
			{
				float BubbleRadius = BubbleMeshComponent->GetStaticMesh()->GetBounds().SphereRadius;
				FVector BubbleScale(1);

				if (MeshComponent->IsA(UStaticMeshComponent::StaticClass()))
				{
					UStaticMesh* StaticMesh = Cast<UStaticMeshComponent>(MeshComponent)->GetStaticMesh();
					float ObjectRadius = StaticMesh->GetBounds().SphereRadius;
					float ObjectCentreOffset = FVector::Distance(FVector(0), StaticMesh->GetBounds().Origin);

					BubbleScale *= (ObjectRadius - ObjectCentreOffset + BubbleDepth) / BubbleRadius;
				}
				else if (MeshComponent->IsA(USkeletalMeshComponent::StaticClass()))
				{
					USkeletalMesh* SkeletalMesh = Cast<USkeletalMeshComponent>(MeshComponent)->SkeletalMesh;
					float ObjectRadius = SkeletalMesh->GetBounds().SphereRadius;
					float ObjectCentreOffset = FVector::Distance(FVector(0), SkeletalMesh->GetBounds().Origin);

					BubbleScale *= (ObjectRadius - ObjectCentreOffset + BubbleDepth) / BubbleRadius;
					GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::White, FString::Printf(TEXT("Object Radius = %f\nBubble Radius = %f\nOffset = %f\nBubble Scale = %f"), ObjectRadius, BubbleRadius, ObjectCentreOffset, BubbleScale.X));

					FVector ObjectBoxExtent = SkeletalMesh->GetBounds().BoxExtent;
					GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::White, FString::Printf(TEXT("Box = %f, %f, %f"), ObjectBoxExtent.X, ObjectBoxExtent.Y, ObjectBoxExtent.Z));
				}
				else
				{
					return;
				}

				NewBubble->SetActorScale3D(BubbleScale);
				NewBubble->SetActorLocation(Object->GetActorLocation());
				NewBubble->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
				Bubble = NewBubble;
			}
		}
	}
}

void AMainCharacter::PopBubble()
{
	if (Bubble)
	{
		Bubble->Destroy();
		Bubble = nullptr;
	}
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
		else if (nearbyObjects[i]->IsA(ADisassemblyTable::StaticClass()))
		{
			hasInteracted = Cast<ADisassemblyTable>(nearbyObjects[i])->ChargeMagic();
		}
		else if (nearbyObjects[i]->IsA(AAssemblingSpot::StaticClass()))
		{
			hasInteracted = Cast<AAssemblingSpot>(nearbyObjects[i])->BeginSewing();
		}
		else if (nearbyObjects[i]->IsA(AResizingDevice::StaticClass()))
		{
			hasInteracted = Cast<AResizingDevice>(nearbyObjects[i])->RemoveFromDevice(this);
		}
		else if (nearbyObjects[i]->IsA(AAnimatingDevice::StaticClass()))
		{
			hasInteracted = Cast<AAnimatingDevice>(nearbyObjects[i])->BeginAnimating();
		}

		if (hasInteracted)
			break;
	}
}

bool AMainCharacter::IsHoldingObject()
{
	return HeldObject != nullptr;
}

void AMainCharacter::OnOverlapBegin(UPrimitiveComponent* OverLappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Fucked.")));
}
