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
#include "Math/TransformNonVectorized.h"

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

bool AMainCharacter::HoldObject(AActor* Object)
{
	if (!Object)
	{
		return false;
	}

	// disable physics on the object
	TArray<UActorComponent*> physicsComponents = Object->GetComponentsByClass(UPrimitiveComponent::StaticClass());
	for (int i = 0; i < physicsComponents.Num(); ++i)
	{
		UPrimitiveComponent* physicsComponent = Cast<UPrimitiveComponent>(physicsComponents[i]);
		physicsComponent->SetSimulatePhysics(false);

		if (physicsComponent != Object->GetRootComponent())
		{
			physicsComponent->AttachToComponent(Object->GetRootComponent(), FAttachmentTransformRules::SnapToTargetIncludingScale);
		}
	}

	if (Object->IsA(ACreature::StaticClass()))
	{
		ACreature* Creature = Cast<ACreature>(Object);

		// attach the object to the player
		Creature->AttachToActor(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale);

		// get the creature's dimension to determine the size offset
		FVector SizeOffset = FVector(Creature->GetDimensions().GetMax());
		SizeOffset.Y = 0;

		// add the total offset to the object
		Creature->SetActorRelativeLocation(PickupPosition + SizeOffset, false, nullptr, ETeleportType::ResetPhysics);
	}
	else if (Object->IsA(ABodyPart::StaticClass()))
	{
		// get the object's skeletal mesh component
		ABodyPart* BodyPart = Cast<ABodyPart>(Object);

		// calculate the mesh offset
		FVector meshCentre = BodyPart->GetMeshRelativeOffset();
		float meshRadius = BodyPart->GetMeshRadius();
		FVector meshOffset = FVector(meshRadius, 0, meshRadius) - meshCentre;

		// attach the object to the player
		Object->AttachToActor(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale);

		// add the total offset to the object
		Object->SetActorRelativeLocation(PickupPosition + meshOffset, false, nullptr, ETeleportType::ResetPhysics);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Invalid object type.")));
		return false;
	}

	ApplyBubble(Object);

	HeldObject = Object;
	return true;
}

void AMainCharacter::ApplyBubble(AActor* Object)
{
	if (!Object || !BubbleBlueprint)
	{
		return;
	}

	float ObjectRadius(0);
	FVector BubbleLocation(0);

	if (Object->IsA(ACreature::StaticClass()))
	{
		ACreature* Creature = Cast<ACreature>(Object);

		ObjectRadius = Creature->GetDimensions().GetAbsMax();
		BubbleLocation = Object->GetRootComponent()->GetRelativeTransform().GetLocation();
	}
	else if (Object->IsA(ABodyPart::StaticClass()))
	{
		ABodyPart* BodyPart = Cast<ABodyPart>(Object);

		ObjectRadius = BodyPart->GetMeshRadius();
		BubbleLocation = Object->GetRootComponent()->GetRelativeTransform().GetLocation() + BodyPart->GetMeshRelativeOffset();
	}
	else
	{
		return;
	}

	AActor* NewBubble = GetWorld()->SpawnActor(BubbleBlueprint);

	if (NewBubble)
	{
		UStaticMeshComponent* BubbleMeshComponent = Cast<UStaticMeshComponent>(NewBubble->GetComponentByClass(UStaticMeshComponent::StaticClass()));

		if (BubbleMeshComponent)
		{
			float BubbleRadius = BubbleMeshComponent->GetStaticMesh()->GetBounds().SphereRadius;
			float BubbleScale = (ObjectRadius + BubbleDepth) / BubbleRadius;
			

			NewBubble->AttachToActor(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
			NewBubble->SetActorRelativeScale3D(FVector(BubbleScale));
			NewBubble->SetActorRelativeLocation(BubbleLocation);
			
			Bubble = NewBubble;
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

FVector AMainCharacter::CalculateObjectCentre(AActor* Object)
{
	if (Object)
	{
		FVector ObjectCentre(0);

		// get the mesh component of the object
		UMeshComponent* ObjectMeshComponent = Cast<UMeshComponent>(Object->GetComponentByClass(UMeshComponent::StaticClass()));
		
		// set the mesh component as the initial value
		USceneComponent* CurrentComponent = ObjectMeshComponent;

		// go up through the component hierarchy and offset each component's relative location
		while (CurrentComponent != Object->GetRootComponent())
		{
			ObjectCentre -= CurrentComponent->RelativeLocation;
			CurrentComponent = CurrentComponent->GetAttachParent();
		}

		if (ObjectMeshComponent->IsA(UStaticMeshComponent::StaticClass()))
		{
			
		}
		else if (ObjectMeshComponent->IsA(USkeletalMeshComponent::StaticClass()))
		{
			// get the object's skeletal mesh from its mesh component
			USkeletalMesh* SkeletalMesh = Cast<USkeletalMeshComponent>(ObjectMeshComponent)->SkeletalMesh;
		}
	}

	return FVector();
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
