// Fill out your copyright notice in the Description page of Project Settings.
#include "MainCharacter.h"
#include "AssemblingTable.h"
#include "BodyPart.h"
#include "BodyStorage.h"
#include "DisassemblingTable.h"
#include "FullBody.h"
#include "Camera/CameraComponent.h"
#include "Components/SceneComponent.h"
#include "Components/InputComponent.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"

// Sets default values
AMainCharacter::AMainCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//// Create the camera arm
	//CameraArm = CreateDefaultSubobject<USpringArmComponent>("Camera Arm");
	//CameraArm->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	//CameraArm->bUsePawnControlRotation = false;

	//// Create the camera
	//Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	//Camera->AttachToComponent(CameraArm, FAttachmentTransformRules::KeepRelativeTransform, USpringArmComponent::SocketName);
	//Camera->bUsePawnControlRotation = false;

	// Create sphere collider
	SphereCollider = CreateDefaultSubobject<USphereComponent>("SphereCollider");
	SphereCollider->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	bUseControllerRotationRoll = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0, RotationSpeed, 0);
}

// Called when the game starts or when spawned
void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();
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

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("PickUp", IE_Pressed, this, &AMainCharacter::PickUpAndDrop);
	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &AMainCharacter::Interact);

	SphereCollider->OnComponentBeginOverlap.AddDynamic(this, &AMainCharacter::OnOverlapBegin);
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

	if (objectToHold->IsA(AFullBody::StaticClass()))
	{
		ACharacter* character = Cast<ACharacter>(objectToHold);

		float bodyHalfHeight = character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

		// attach the object to the player
		character->AttachToActor(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		
		// add offset to the object
		character->SetActorRelativeLocation(FVector(0, 0, bodyHalfHeight) + PickupOffset, false, nullptr, ETeleportType::ResetPhysics);
	}
	else if (objectToHold->IsA(ABodyPart::StaticClass()))
	{
		// get the object's skeletal mesh component
		USkeletalMeshComponent* skMeshComponent = Cast<USkeletalMeshComponent>(objectToHold->GetComponentByClass(USkeletalMeshComponent::StaticClass()));

		// if the object has skeletal mesh
		if (skMeshComponent == nullptr || skMeshComponent->SkeletalMesh == nullptr)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Skeletal mesh not found.")));
			return false;
		}

		// calculate the mesh offset
		FBoxSphereBounds meshBounds = skMeshComponent->SkeletalMesh->GetBounds();
		FVector meshCentre = meshBounds.Origin;
		float meshHalfHeight = meshBounds.SphereRadius;
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

void AMainCharacter::MoveForward(float Axis)
{
	// Find out which way is "forward" and record that the player wants to move that way.
	FRotator Rotation = Controller->GetControlRotation();
	FRotator YawRotation(0, Rotation.Yaw, 0);
	FVector Direction = FRotationMatrix(YawRotation).GetScaledAxis(EAxis::X);
	AddMovementInput(Direction, Axis);
}

void AMainCharacter::MoveRight(float Axis)
{
	// Find out which way is "right" and record that the player wants to move that way.
	FRotator Rotation = Controller->GetControlRotation();
	FRotator YawRotation(0, Rotation.Yaw, 0);
	FVector Direction = FRotationMatrix(YawRotation).GetScaledAxis(EAxis::Y);
	AddMovementInput(Direction, Axis);
}

void AMainCharacter::PickUpAndDrop()
{
	// get all nearby objects
	TArray<AActor*> nearbyObjects;
	SphereCollider->GetOverlappingActors(nearbyObjects);

	// if the player is not holding anything
	if (heldObject == nullptr)
	{
		// check all nearby objects
		for (int i = 0; i < nearbyObjects.Num(); ++i)
		{
			AActor* objectToHold = nearbyObjects[i];

			if (objectToHold->IsA(ABodyStorage::StaticClass()))
			{
				HoldObject(Cast<ABodyStorage>(nearbyObjects[i])->TakeBody());
				break;
			}

			if (objectToHold->IsA(ABodyPart::StaticClass()))
			{
				ABodyPart* aBodyPart = Cast<ABodyPart>(objectToHold);

				// if the body part is attached to a body
				if (aBodyPart->body != nullptr)
				{
					// set the object to hold as the full body
					objectToHold = aBodyPart->body;
				}
			}

			// get the object's attach parent
			AActor* objectAttachParent = objectToHold->GetAttachParentActor();

			// if the object has an attach parent
			if (objectAttachParent != nullptr)
			{
				if (objectAttachParent->IsA(AAssemblingTable::StaticClass()))
				{
					// remove it from the assembling table
					Cast<AAssemblingTable>(objectAttachParent)->RemoveFromTable(objectToHold);
				}
				else if (objectAttachParent->IsA(ADisassemblingTable::StaticClass()))
				{
					// remove it from the disassembling table
					Cast<ADisassemblingTable>(objectAttachParent)->RemoveFromTable();
				}
			}

			// get player to hold the object
			HoldObject(objectToHold);
			break;
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
			// if the assembling table is nearby
			if (nearbyObjects[i]->ActorHasTag("AssemblingTable"))
			{
				// call the table's object snapping function
				AAssemblingTable* aTable = Cast<AAssemblingTable>(nearbyObjects[i]);
				isSnapped = aTable->DropToTable(heldObject);
				break;
			}
			else if (nearbyObjects[i]->ActorHasTag("DisassemblingTable"))
			{
				// call the table's object snapping function
				ADisassemblingTable* dTable = Cast<ADisassemblingTable>(nearbyObjects[i]);
				isSnapped = dTable->DropToTable(heldObject);
				break;
			}
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

void AMainCharacter::Interact()
{
	TArray<AActor*> actors;
	SphereCollider->GetOverlappingActors(actors);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Interacting...")));
	for (int i = 0; i < actors.Num(); ++i)
	{
		if (actors[i]->IsA(ADisassemblingTable::StaticClass()))
		{
			ADisassemblingTable* dTable = Cast<ADisassemblingTable>(actors[i]);
			dTable->Charge();
			break;
		}

		if (actors[i]->IsA(AAssemblingTable::StaticClass()))
		{
			AAssemblingTable* aTable = Cast<AAssemblingTable>(actors[i]);
			aTable->StartMinigame();
			break;
		}
	}
}

void AMainCharacter::OnOverlapBegin(UPrimitiveComponent* OverLappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Overlapped.")));
}
